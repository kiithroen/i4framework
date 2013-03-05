#include "stdafx.h"
#include "I4ObjectRigidBodyComponent.h"
#include "I4Log.h"
#include "I4ModelMgr.h"
#include "I4Model.h"
#include "I4Renderer.h"

namespace i4object {

	I4ObjectRigidBodyComponent::I4ObjectRigidBodyComponent(void)
	{
	}


	I4ObjectRigidBodyComponent::~I4ObjectRigidBodyComponent(void)
	{
	}

	void I4ObjectRigidBodyComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onPostSimulate"), this, bind(&I4ObjectRigidBodyComponent::onPostSimulate, this, _1));
	}

	void I4ObjectRigidBodyComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onPostSimulate"), this);
	}


	void I4ObjectRigidBodyComponent::onPostSimulate(I4MessageArgs& args)
	{
		if (body == nullptr)
			return;

		btTransform worldTM;
		body->getMotionState()->getWorldTransform(worldTM);

		I4Matrix4x4 matWorldTM;
		btTransformToI4Matrix4x4(matWorldTM, worldTM);

		getOwner()->setLocalTM(matWorldTM);
	}

	void I4ObjectRigidBodyComponent::attachBox(const btVector3& ext, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		btTransform t;
		I4Matrix4x4TobtTransform(t, getOwner()->getWorldTM());

		body = getOwner()->getObjectMgr()->getBulletPhysics()->createBox(t, ext, mass, restitution, friction, linDamping, angDamping);
	}

}