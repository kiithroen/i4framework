#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

#include "I4Vector3.h"
#include "I4Vector4.h"
#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics
{
	class Renderer;
}
using namespace i4graphics;

namespace i4object
{
	inline void convertTo(Vector4& color, const PxU32 rgba)
	{
		const float inv255 = 1.0f / 255.0f;

		color.b = (float)((rgba>>16) & 0xff)*inv255;
		color.g = (float)((rgba>>8)  & 0xff)*inv255;
		color.r = (float)((rgba)     & 0xff)*inv255;
		color.a = 1.0f;
	}

	inline void convertTo(Vector3& out, const PxVec3& vec)
	{
		out.x = vec.x;
		out.y = vec.y;
		out.z = vec.z;
	}

	inline void convertTo(PxVec3& out, const Vector3& vec)
	{
		out.x = vec.x;
		out.y = vec.y;
		out.z = vec.z;
	}

	inline void convertTo(Matrix4x4& out, const physx::PxMat44& mat)
	{
		memcpy(&out._11, mat.front(), 4 * 4 * sizeof (float));
	}

	inline void convertTo(physx::PxMat44& out, const Matrix4x4& mat)
	{
		memcpy(&out.column0.x, &mat._11, 4 * 4 * sizeof (float));
	}


	class PvdConnectionHandler : public PVD::PvdConnectionHandler
	{
	public:
		PvdConnectionHandler(PxPhysics* physx)
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


	class PhysXMgr
	{
	public:
		PhysXMgr(void);
		~PhysXMgr(void);

		bool init();

		void enableVisualization(bool enable);

		void destroy();

		void simulate(PxReal dt);
		void commitDebugToRenderer(Renderer* renderer);

		PxActor* createPlane();
		PxRigidDynamic* createBox(const PxTransform& transform, const Vector3& ext, float density);
		PxRigidDynamic* createSphere(const PxTransform& transform, float radius, float density);
		PxRigidDynamic* createCapsule(const PxTransform& transform, float radius, float height, float density);
		PxRigidDynamic* createRepX(const PxTransform& transform, const char* fname, float density);

		PxController* createCapsuleController(const Vector3& p, float radius, float height, float slopeLimit, float stepOffset, PxUserControllerHitReport* hitCallback, PxControllerBehaviorCallback* behaviorCallback);

	private:
		void enablePvdConnection(bool enable);
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
		PvdConnectionHandler*			mPvdConnectionHandler;
		PxControllerManager*			mControllerManager;
		float							mAccumulator;
	};
}

