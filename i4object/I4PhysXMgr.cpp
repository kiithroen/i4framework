#include "stdafx.h"
#include "I4PhysXMgr.h"
#include "I4Log.h"
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
#pragma comment(lib, "RepX3DEBUG.lib")
#pragma comment(lib, "RepXUpgrader3DEBUG.lib")

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
#pragma comment(lib, "RepX3PROFILE.lib")
#pragma comment(lib, "RepXUpgrader3DPROFILE.lib")

#else

#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysX3Cooking_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#pragma comment(lib, "PxTask.lib")
#pragma comment(lib, "RepX3.lib")
#pragma comment(lib, "RepXUpgrader3.lib")

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

	PhysXMgr::PhysXMgr(void)
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
	{
	}

	PhysXMgr::~PhysXMgr()
	{
		destroy();
	}

	bool PhysXMgr::init()
	{
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if(!mFoundation)
		{
			LOG_ERROR << "PxCreateFoundation failed!";
			return false;
		}
		/*
		mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
		if(!mProfileZoneManager)
		{
			LOG_ERROR << "PxProfileZoneManager::createProfileZoneManager failed!";
			return false;
		}
		*/
#ifdef PX_WINDOWS
		// 뻗을때도 있고 안뻗을때도 있는데... 어차피 내 카드는 ATI.. 돌아가지도 않는거 일단 주석처리
		/*
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
		*/
#endif
		bool recordMemoryAllocations = true;
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations, mProfileZoneManager );
		if(!mPhysics)
		{
			LOG_ERROR << "PxCreatePhysics failed!";
			return false;
		}

		if (!PxInitExtensions(*mPhysics))
		{
			LOG_ERROR << "PxInitExtensions failed!";
			return false;
		}
		
		mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams());
		if (!mCooking)
		{
			LOG_ERROR << "PxCreateCooking failed!";
			return false;
		}

		if(mPhysics->getPvdConnectionManager())
		{
			mPvdConnectionHandler = new PvdConnectionHandler(mPhysics);
			mPhysics->getPvdConnectionManager()->addHandler(*mPvdConnectionHandler);
		}

		PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

		if(!sceneDesc.cpuDispatcher)
		{
			mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			if(!mCpuDispatcher)
			{
				LOG_ERROR << "PxDefaultCpuDispatcherCreate failed!";
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
			LOG_ERROR << "createScene failed!";
			return false;
		}
				
		mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution
		if(!mMaterial)
		{
			LOG_ERROR << "createMaterial failed!";
			return false;
		};

		mControllerManager = PxCreateControllerManager(*mFoundation);

		return true;
	}

	void PhysXMgr::destroy()
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

	void PhysXMgr::simulate(PxReal dt)
	{
		mScene->simulate(dt);

		while (!mScene->fetchResults())
		{
			// do something useful
		}
	}

	PxActor* PhysXMgr::createPlane()
	{
		PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f),PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
		PxRigidStatic* plane = mPhysics->createRigidStatic(pose);
		if (!plane)
		{
			LOG_WARN << "create plane failed!";
			return nullptr;
		}

		PxShape* shape = plane->createShape(PxPlaneGeometry(), *mMaterial);
		if (!shape)
		{
			LOG_WARN << "create shape failed!";
			return nullptr;
		}

		mScene->addActor(*plane);

		return plane;
	}

	PxRigidDynamic* PhysXMgr::createBox(const PxTransform& transform, const Vector3& ext, float density)
	{
		PxRigidDynamic* actor = PxCreateDynamic(*mPhysics, transform, PxBoxGeometry(PxVec3(ext.x, ext.y, ext.z)), *mMaterial, density);
		if (!actor)
		{
			LOG_WARN << "create box failed!";
		}

		mScene->addActor(*actor);

		return actor;
	}



	PxRigidDynamic* PhysXMgr::createSphere(const PxTransform& transform, float radius, float density)
	{
		PxRigidDynamic* actor = PxCreateDynamic(*mPhysics, transform, PxSphereGeometry(radius), *mMaterial, density);
		if (!actor)
		{
			LOG_WARN << "create sphere failed!";
		}

		mScene->addActor(*actor);

		return actor;
	}

	PxRigidDynamic* PhysXMgr::createCapsule(const PxTransform& transform, float radius, float height, float density)
	{
		// Y가 업이 되도록 Z축을 기준으로 90도 돌린다.(기본 캡슐은 X축으로 누워있다.)
		PxRigidDynamic* actor = PxCreateDynamic(*mPhysics, transform, PxCapsuleGeometry(radius, height*0.5f), *mMaterial, density, PxTransform(PxQuat(PI/2, PxVec3(0, 0, 1))));
		if (!actor)
		{
			LOG_WARN << "create capsule failed!";
		}

		mScene->addActor(*actor);

		return actor;
	}

	PxCapsuleController* PhysXMgr::createCapsuleController(const Vector3& p, float radius, float height, float slopeLimit, float stepOffset, PxUserControllerHitReport* hitCallback, PxControllerBehaviorCallback* behaviorCallback)
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

		PxCapsuleController* c = (PxCapsuleController*)mControllerManager->createController(*mPhysics, mScene, desc);
		if (!c)
		{
			LOG_WARN << "create controller failed!";
		}

		return c;
	}

	PxRigidDynamic* PhysXMgr::createRepX(const PxTransform& transform, const char* fname, float density)
	{
		PxRigidDynamic* rigid = nullptr;
		PxCollection* buffers = mPhysics->createCollection();
		PxCollection* objects = mPhysics->createCollection();

		PxDefaultFileInputData data(fname);
		if (data.isValid() == false)
			return nullptr;

		repx::deserializeFromRepX(data, *mPhysics, *mCooking, nullptr, nullptr, *buffers, *objects, nullptr);

		// 씬 속성들을 덮어버리는 경우가 있어서 전부 포함하지말고 액터만 추가하도록 했다.
		//mPhysics->addCollection( *buffers, *mScene );
		//mPhysics->addCollection( *objects, *mScene );

		for (PxU32 i = 0; i < objects->getNbObjects(); i++ )
		{
			PxRigidDynamic* actor = objects->getObject(i)->is<PxRigidDynamic>();
			if (actor)
			{
				actor->setGlobalPose(transform, true);

				// 맥스 플러그인에서 뽑아져온걸 다렉 좌표계에 맞게 돌려준다.
				// TODO : 느리기도 하고 귀찮은게 XML 을 직접 파싱하는게 나을지도...
				PxShape** shapes = new PxShape*[actor->getNbShapes()];
				actor->getShapes(shapes, actor->getNbShapes());
				for (PxU32 i = 0; i < actor->getNbShapes(); ++i)
				{
					shapes[i]->setLocalPose(PxTransform(PxQuat(-PI/2, PxVec3(1, 0, 0))));
				}

				PxRigidBodyExt::updateMassAndInertia(*actor, 1.0f);
				mScene->addActor(*actor);

				delete[] shapes;

				rigid = actor;
			}
		}

		objects->release();
		buffers->release();

		return rigid;
	}

	bool PhysXMgr::sweepSphere(const Vector3& p, float radius, const Vector3& dir, float dist)
	{
		PxSweepHit hit;
		const PxSceneQueryFlags outputFlags = PxSceneQueryFlag::eDISTANCE;
		return mScene->sweepAny(PxSphereGeometry(radius), PxTransform(PxVec3(p.x, p.y, p.z)), PxVec3(dir.x, dir.y, dir.z), dist, outputFlags, hit);
	}

	void PhysXMgr::enablePvdConnection(bool enable)
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

	void PhysXMgr::createPvdConnection()
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

	void PhysXMgr::commitDebugToRenderer(Renderer* renderer)
	{
		DebugLine l;

		const PxDebugLine* lines = mScene->getRenderBuffer().getLines();
		PxU32 numLine = mScene->getRenderBuffer().getNbLines();
				
		for (PxU32 i = 0; i < numLine; ++i)
		{
			convertTo(l.color, lines[i].color0);

			convertTo(l.p0, lines[i].pos0);
			convertTo(l.p1, lines[i].pos1);
			
			renderer->commit(l);
		}

		const PxDebugTriangle* triangles = mScene->getRenderBuffer().getTriangles();
		PxU32 numTriangle = mScene->getRenderBuffer().getNbTriangles();
		for (PxU32 i = 0; i < numTriangle; ++i)
		{
			convertTo(l.color, triangles[i].color0);

			convertTo(l.p0, triangles[i].pos0);
			convertTo(l.p1, triangles[i].pos1);

			renderer->commit(l);

			convertTo(l.p0, triangles[i].pos1);
			convertTo(l.p1, triangles[i].pos2);

			renderer->commit(l);

			convertTo(l.p0, triangles[i].pos2);
			convertTo(l.p1, triangles[i].pos0);

			renderer->commit(l);
		}
		
		// point 는 필요가 없어서 미구현. 혹시 중요한 정보가 생기면 그때 구현하도록 한다.
	}

	void PhysXMgr::enableVisualization(bool enable)
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
