#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4PhysXMgr.h"
using namespace i4core;

namespace i4object
{
	class I4ObjectRigidBodyComponent : public I4ObjectComponent
	{
	public:
		I4ObjectRigidBodyComponent();
		virtual ~I4ObjectRigidBodyComponent(void);

		static const char*	getComponentID()	{ return "RigidBody"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;
		
		void setOffset(const I4Matrix4x4& m);
		void setKinematic(bool isKinematic);

		void attachBox(const I4Vector3& ext, float density, bool kinematic);
		void attachSphere(float radius, float density, bool kinematic);
		void attachCapsule(float radius, float height, float density, bool kinematic);

		void onPreSimulate(I4MessageArgs& args);
		void onPostSimulate(I4MessageArgs& args);

	private:
		void WorldTM2PxTransform(PxTransform& transform);
		void PxTransform2WorldTM(I4Matrix4x4& matWorldTM);

	private:
		I4Matrix4x4			matOffset;
		PxRigidDynamic*		body;
	};
}