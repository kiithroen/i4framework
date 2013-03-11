#include "stdafx.h"
#include "I4PhysXMgr.h"
#include "I4Log.h"
#include "I4Profile.h"
#include "I4Renderer.h"
using namespace i4core;

//#define PHYSX_PROFILE

#ifdef _DEBUG

#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#pragma comment(lib, "PxTaskDEBUG.lib")

#else

#ifdef PHYSX_PROFILE

#pragma comment(lib, "PhysX3PROFILE_x86.lib")
#pragma comment(lib, "PhysX3CommonPROFILE_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsPROFILE.lib")
#pragma comment(lib, "PhysX3CookingPROFILE_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematicPROFILE_x86.lib")
#pragma comment(lib, "PhysXProfileSDKPROFILE.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKPROFILE.lib")
#pragma comment(lib, "PxTaskPROFILE.lib")

#else

#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysX3Cooking_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PxTask.lib")

#endif

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
		, mCpuDispatcher(nullptr)
		, mCudaContextManager(nullptr)
		, mPvdConnectionHandler(nullptr)
		, mControllerManager(nullptr)
		, mAccumulator(0)
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

		if(mPhysics->getPvdConnectionManager())
		{
			mPvdConnectionHandler = new I4PvdConnectionHandler(mPhysics);
			mPhysics->getPvdConnectionManager()->addHandler(*mPvdConnectionHandler);
		}

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

		mControllerManager = PxCreateControllerManager(*mFoundation);

		return true;
	}

	void I4PhysXMgr::destroy()
	{
		if (mControllerManager)
		{
			mControllerManager->release();
			mControllerManager = nullptr;
		}

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

	void I4PhysXMgr::simulate(PxReal dt)
	{
		I4PROFILE_THISFUNC;

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

	PxRigidDynamic* I4PhysXMgr::createBox(const PxTransform& transform, const I4Vector3& ext, float density)
	{
		PxRigidDynamic* actor = PxCreateDynamic(*mPhysics, transform, PxBoxGeometry(PxVec3(ext.x, ext.y, ext.z)), *mMaterial, density);
		if (!actor)
		{
			I4LOG_WARN << "create box failed!";
		}

		mScene->addActor(*actor);

		return actor;
	}



	PxRigidDynamic* I4PhysXMgr::createSphere(const PxTransform& transform, float radius, float density)
	{
		PxRigidDynamic* actor = PxCreateDynamic(*mPhysics, transform, PxSphereGeometry(radius), *mMaterial, density);
		if (!actor)
		{
			I4LOG_WARN << "create sphere failed!";
		}

		mScene->addActor(*actor);

		return actor;
	}

	PxRigidDynamic* I4PhysXMgr::createCapsule(const PxTransform& transform, float radius, float height, float density)
	{
		// Y가 업이 되도록 Z축을 기준으로 90도 돌린다.(기본 캡슐은 X축으로 누워있다.)
		PxRigidDynamic* actor = PxCreateDynamic(*mPhysics, transform, PxCapsuleGeometry(radius, height*0.5f), *mMaterial, density, PxTransform(PxQuat(I4PI/2, PxVec3(0, 0, 1))));
		if (!actor)
		{
			I4LOG_WARN << "create capsule failed!";
		}

		mScene->addActor(*actor);

		return actor;
	}

	PxController* I4PhysXMgr::createCapsuleController(const I4Vector3& p, float radius, float height, float slopeLimit, float stepOffset, PxUserControllerHitReport* hitCallback, PxControllerBehaviorCallback* behaviorCallback)
	{
		PxCapsuleControllerDesc desc;		
		desc.setToDefault();
		desc.radius = radius;
		desc.height = height;
		desc.position.set(p.x, p.y, p.z);
		desc.slopeLimit = slopeLimit;
		desc.stepOffset = stepOffset;
		desc.material = mMaterial;
		desc.callback = hitCallback;
		desc.behaviorCallback = behaviorCallback;

		PxController* c = mControllerManager->createController(*mPhysics, mScene, desc);
		if (!c)
		{
			I4LOG_WARN << "create controller failed!";
		}

		return c;
	}

	void I4PhysXMgr::enablePvdConnection(bool enable)
	{
		if(!mPhysics->getPvdConnectionManager()) return;

		if (enable)
		{
			if (mPhysics->getPvdConnectionManager()->isConnected() == false)
			{
				createPvdConnection();
			}
		}
		else
		{
			if (mPhysics->getPvdConnectionManager()->isConnected())
			{
				mPhysics->getPvdConnectionManager()->disconnect();
			}
		}
	}

	void I4PhysXMgr::createPvdConnection()
	{
		if(mPhysics->getPvdConnectionManager() == NULL)
			return;

		PxVisualDebuggerConnectionFlags theConnectionFlags( PxVisualDebuggerExt::getDefaultConnectionFlags() );

		const char* pvdHost = "127.0.0.1";
		PxU32 pvdPort = 5425;
		
		PVD::PvdConnection* theConnection = PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(), pvdHost, pvdPort, 4000, theConnectionFlags );
		if (theConnection)
			theConnection->release();
	}

	void I4PhysXMgr::commitDebugToRenderer(I4Renderer* renderer)
	{
		I4PROFILE_THISFUNC;

		I4DebugLine l;

		const PxDebugLine* lines = mScene->getRenderBuffer().getLines();
		PxU32 numLine = mScene->getRenderBuffer().getNbLines();
				
		for (PxU32 i = 0; i < numLine; ++i)
		{
			convertToI4Color(l.color, lines[i].color0);

			convertToI4Vector3(l.p0, lines[i].pos0);
			convertToI4Vector3(l.p1, lines[i].pos1);
			
			renderer->commit(l);
		}

		const PxDebugTriangle* triangles = mScene->getRenderBuffer().getTriangles();
		PxU32 numTriangle = mScene->getRenderBuffer().getNbTriangles();
		for (PxU32 i = 0; i < numTriangle; ++i)
		{
			convertToI4Color(l.color, triangles[i].color0);

			convertToI4Vector3(l.p0, triangles[i].pos0);
			convertToI4Vector3(l.p1, triangles[i].pos1);

			renderer->commit(l);

			convertToI4Vector3(l.p0, triangles[i].pos1);
			convertToI4Vector3(l.p1, triangles[i].pos2);

			renderer->commit(l);

			convertToI4Vector3(l.p0, triangles[i].pos2);
			convertToI4Vector3(l.p1, triangles[i].pos0);

			renderer->commit(l);
		}
		
		// point 는 필요가 없어서 미구현. 혹시 중요한 정보가 생기면 그때 구현하도록 한다.
	}

	void I4PhysXMgr::enableVisualization(bool enable)
	{
		// 활성화하면 엄청 느려짐!

		float val = 0.0f;
		if (enable)
		{
			val = 1.0f;
		}

		mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, val);
		mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, val);
		mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_STATIC, val);
		mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_COMPOUNDS, val);
		
		enablePvdConnection(enable);
	}


}
