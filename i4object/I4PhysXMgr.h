#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

#include "I4Vector3.h"
#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4object
{
	inline void convertToI4Vector3(I4Vector3& out, const PxVec3& vec)
	{
		out.x = vec.x;
		out.y = vec.y;
		out.z = vec.z;
	}

	inline void convertToPxVec3(PxVec3& out, const I4Vector3& vec)
	{
		out.x = vec.x;
		out.y = vec.y;
		out.z = vec.z;
	}

	inline void convertToI4Matrix4x4(I4Matrix4x4& out, const physx::PxMat44& mat)
	{
		memcpy(&out._11, mat.front(), 4 * 4 * sizeof (float));
	}

	inline void convertToPxMat4x4(physx::PxMat44& out, const I4Matrix4x4& mat)
	{
		memcpy(&out.column0.x, &mat._11, 4 * 4 * sizeof (float));
	}


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


	class I4PhysXMgr
	{
	public:
		I4PhysXMgr(void);
		~I4PhysXMgr(void);

		bool init();
		void destroy();

		void simulate(PxReal dt);

		PxActor* createPlane();
		PxRigidDynamic* createBox(const PxTransform& transform, const I4Vector3& ext, float density);
		PxRigidDynamic* createSphere(const PxTransform& transform, float radius, float density);
		PxRigidDynamic* createCapsule(const PxTransform& transform, float radius, float height, float density);
		PxController* createCapsuleController(const I4Vector3& p, float radius, float height, float slopeLimit, float stepOffset, PxUserControllerHitReport* hitCallback, PxControllerBehaviorCallback* behaviorCallback);

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
		PxControllerManager*			mControllerManager;
		float							mAccumulator;
	};
}

