#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

namespace i4object
{
	class I4PvdConnectionHandler : public PVD::PvdConnectionHandler
	{
	public:
		I4PvdConnectionHandler(PxPhysics* physx)
			: mPhysics(physx)
		{

		}
		void onPvdConnected(PVD::PvdConnection& connection)
		{
			mPhysics->getVisualDebugger()->setVisualizeConstraints(true);
			mPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::eTRANSMIT_CONTACTS, true);
			mPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::eTRANSMIT_SCENEQUERIES, true);
		}

		void onPvdDisconnected(PVD::PvdConnection& connection)
		{
		}

		void onPvdSendClassDescriptions(PVD::PvdConnection& connection )
		{

		}

	private:
		PxPhysics*						mPhysics;
	};


	class I4OBJECT_API I4PhysXMgr
	{
	public:
		I4PhysXMgr(void);
		~I4PhysXMgr(void);

		bool init();
		void destroy();

		void step(PxReal dt);

		PxActor* createPlane();
		PxActor* createCube();

	private:
		void togglePvdConnection();
		void createPvdConnection();

	private:
		PxFoundation*					mFoundation;
		PxProfileZoneManager*			mProfileZoneManager;
		PxPhysics*						mPhysics;
		PxCooking*						mCooking;
		PxScene*						mScene;
		PxMaterial*						mMaterial;

		PxDefaultCpuDispatcher*			mCpuDispatcher;
#ifdef PX_WINDOWS
		pxtask::CudaContextManager*		mCudaContextManager;
#endif
		I4PvdConnectionHandler*			mPvdConnectionHandler;
	};
}
