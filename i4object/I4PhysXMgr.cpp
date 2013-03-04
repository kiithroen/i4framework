#include "stdafx.h"
#include "I4PhysXMgr.h"
#include "I4Log.h"
using namespace i4core;



#ifdef _DEBUG
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#pragma comment(lib, "PxTaskDEBUG.lib")
#else
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysX3Cooking_x86.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PxTask.lib")
#endif

namespace i4object
{
	static PxDefaultErrorCallback			gDefaultErrorCallback;
	static PxDefaultAllocator				gDefaultAllocatorCallback;

	PX_FORCE_INLINE PxSimulationFilterShader getDefaultFilterShader()
	{
		return PxDefaultSimulationFilterShader;
	}

	I4PhysXMgr::I4PhysXMgr(void)
		: mFoundation(nullptr)
		, mProfileZoneManager(nullptr)
		, mPhysics(nullptr)
		, mCooking(nullptr)
		, mScene(nullptr)
		, mMaterial(nullptr)
	{
	}

	I4PhysXMgr::~I4PhysXMgr()
	{
		destroy();
	}

	bool I4PhysXMgr::init()
	{
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if(!mFoundation)
		{
			I4LOG_ERROR << "PxCreateFoundation failed!";
			return false;
		}
		/*
		mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
		if(!mProfileZoneManager)
		{
			I4LOG_ERROR << "PxProfileZoneManager::createProfileZoneManager failed!";
			return false;
		}
		*/
#ifdef PX_WINDOWS
		pxtask::CudaContextManagerDesc cudaContextManagerDesc;
		mCudaContextManager = pxtask::createCudaContextManager(*mFoundation, cudaContextManagerDesc, mProfileZoneManager);
		if( mCudaContextManager )
		{
			if( !mCudaContextManager->contextIsValid() )
			{
				mCudaContextManager->release();
				mCudaContextManager = NULL;
			}
		}
#endif
		bool recordMemoryAllocations = true;
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations, mProfileZoneManager );
		if(!mPhysics)
		{
			I4LOG_ERROR << "PxCreatePhysics failed!";
			return false;
		}

		if (!PxInitExtensions(*mPhysics))
		{
			I4LOG_ERROR << "PxInitExtensions failed!";
			return false;
		}
		
		mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams());
		if (!mCooking)
		{
			I4LOG_ERROR << "PxCreateCooking failed!";
			return false;
		}

		togglePvdConnection();

		mPvdConnectionHandler = new I4PvdConnectionHandler(mPhysics);
		if(mPhysics->getPvdConnectionManager())
			mPhysics->getPvdConnectionManager()->addHandler(*mPvdConnectionHandler);

		PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

		if(!sceneDesc.cpuDispatcher)
		{
			mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			if(!mCpuDispatcher)
			{
				I4LOG_ERROR << "PxDefaultCpuDispatcherCreate failed!";
				return false;
			}

			sceneDesc.cpuDispatcher    = mCpuDispatcher;
		}

		if(!sceneDesc.filterShader)
			sceneDesc.filterShader    = getDefaultFilterShader();

#ifdef PX_WINDOWS
		if(!sceneDesc.gpuDispatcher && mCudaContextManager)
		{
			sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
		}
#endif

		mScene = mPhysics->createScene(sceneDesc);
		if (!mScene)
		{
			I4LOG_ERROR << "createScene failed!";
			return false;
		}

		mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution
		if(!mMaterial)
		{
			I4LOG_ERROR << "createMaterial failed!";
			return false;
		};

		return true;
	}

	void I4PhysXMgr::destroy()
	{
		if (mScene)
		{
			mScene->release();
			mScene = nullptr;
		}

		if (mCpuDispatcher)
		{
			mCpuDispatcher->release();
			mCpuDispatcher = nullptr;
		}

		if (mCooking)
		{
			mCooking->release();
			mCooking = nullptr;
		}

		PxCloseExtensions();

		if (mPhysics)
		{
			mPhysics->release();
			mPhysics = nullptr;
		}

		delete mPvdConnectionHandler;

#ifdef PX_WINDOWS
		if (mCudaContextManager)
		{
			mCudaContextManager->release();
			mCudaContextManager = nullptr;
		}
#endif
		if (mProfileZoneManager)
		{
			mProfileZoneManager->release();
			mProfileZoneManager = nullptr;
		}

		if (mFoundation)
		{
			mFoundation->release();
			mFoundation = nullptr;
		}
	}

	void I4PhysXMgr::step(PxReal dt)
	{
		mScene->simulate(dt);

		while (!mScene->fetchResults())
		{
			// do something useful
		}
	}

	PxActor* I4PhysXMgr::createPlane()
	{
		PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f),PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
		PxRigidStatic* plane = mPhysics->createRigidStatic(pose);
		if (!plane)
		{
			I4LOG_WARN << "create plane failed!";
			return nullptr;
		}

		PxShape* shape = plane->createShape(PxPlaneGeometry(), *mMaterial);
		if (!shape)
		{
			I4LOG_WARN << "create shape failed!";
			return nullptr;
		}

		mScene->addActor(*plane);

		return plane;
	}

	PxActor* I4PhysXMgr::createCube()
	{
		PxReal density = 1.0f;
		PxTransform transform(PxVec3(0.0f, 10.0f, 0.0f), PxQuat::createIdentity());
		PxVec3 dimensions(0.5,0.5,0.5);
		PxBoxGeometry geometry(dimensions);

		PxRigidDynamic *actor = PxCreateDynamic(*mPhysics, transform, geometry, *mMaterial, density);
		actor->setAngularDamping(0.75);
		actor->setLinearVelocity(PxVec3(0,0,0)); 
		if (!actor)
		{
			I4LOG_WARN << "create actor failed!";
		}

		mScene->addActor(*actor);

		return actor;
	}

	void I4PhysXMgr::togglePvdConnection()
	{
		if(!mPhysics->getPvdConnectionManager()) return;
		if (mPhysics->getPvdConnectionManager()->isConnected())
			mPhysics->getPvdConnectionManager()->disconnect();
		else
			createPvdConnection();
	}

	void I4PhysXMgr::createPvdConnection()
	{
		if(mPhysics->getPvdConnectionManager() == NULL)
			return;

		PxVisualDebuggerConnectionFlags theConnectionFlags( PxVisualDebuggerExt::getAllConnectionFlags() );

		const char* pvdHost = "127.0.0.1";
		PxU32 pvdPort = 5425;
		
		PVD::PvdConnection* theConnection = PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(), pvdHost, pvdPort, 4000, theConnectionFlags );
		if (theConnection)
			theConnection->release();
	}

	
}
