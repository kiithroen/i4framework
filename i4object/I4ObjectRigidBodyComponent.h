#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4BulletPhysics.h"
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

		void attachBox(const btVector3& ext, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping);
		void attachSphere(btScalar radius, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping);
		void attachCapsule(btScalar radius, btScalar height, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping);

		void onPreSimulate(I4MessageArgs& args);
		void onPostSimulate(I4MessageArgs& args);

	private:
		void btTransform2objectTM(I4Matrix4x4& matWorldTM);
		void objectTM2btTransform(btTransform& transform);
	private:
		I4Matrix4x4			matOffset;
		btRigidBody*		body;
	};
}