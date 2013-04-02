#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4PhysXMgr.h"
using namespace i4core;

namespace i4object
{
	class ObjectRigidBodyComponent : public ObjectComponent
	{
	public:
		ObjectRigidBodyComponent();
		virtual ~ObjectRigidBodyComponent(void);

		static const char*	getComponentID()	{ return "RigidBody"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;
		
		void setOffset(const Matrix4x4& m);
		void setKinematic(bool isKinematic);

		void attachBox(const Vector3& ext, float density, bool kinematic);
		void attachSphere(float radius, float density, bool kinematic);
		void attachCapsule(float radius, float height, float density, bool kinematic);

		void onPreSimulate(MessageArgs& args);
		void onPostSimulate(MessageArgs& args);

	private:
		void WorldTM2PxTransform(PxTransform& transform);
		void PxTransform2WorldTM(Matrix4x4& matWorldTM);

	private:
		Matrix4x4			matOffset;
		PxRigidDynamic*		body;
	};
}