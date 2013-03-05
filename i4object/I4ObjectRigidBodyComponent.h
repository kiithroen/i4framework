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

		virtual const char*		getComponentID()	{ return "RigidBody"; }

		virtual void			onAdd() override;
		virtual void			onRemove() override;
		
		void attachBox(const btVector3& ext, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping);

		void onPostSimulate(I4MessageArgs& args);

	private:
		btRigidBody*		body;
	};
}