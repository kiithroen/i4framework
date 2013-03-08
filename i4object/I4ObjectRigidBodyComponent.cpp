#include "stdafx.h"
#include "I4ObjectRigidBodyComponent.h"
#include "I4Log.h"
#include "I4ModelMgr.h"
#include "I4Model.h"
#include "I4Renderer.h"
#include "I4Profile.h"

namespace i4object {

	I4ObjectRigidBodyComponent::I4ObjectRigidBodyComponent(void)
		: matOffset(I4MATRIX4X4_IDENTITY)
		, body(nullptr)
	{
	}


	I4ObjectRigidBodyComponent::~I4ObjectRigidBodyComponent(void)
	{
	}

	void I4ObjectRigidBodyComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onSyncSimulate"), this, bind(&I4ObjectRigidBodyComponent::onSyncSimulate, this, _1));
	}

	void I4ObjectRigidBodyComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onSyncSimulate"), this);
	}


	void I4ObjectRigidBodyComponent::onSyncSimulate(I4MessageArgs& args)
	{
		I4PROFILE_THISFUNC;

		if (body == nullptr)
			return;

		I4Matrix4x4 matWorldTM;
		btTransform2objectTM(matWorldTM);

		getOwner()->setLocalTM(matWorldTM);
	}

	void I4ObjectRigidBodyComponent::attachBox(const btVector3& ext, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		btTransform transform;
		objectTM2btTransform(transform);
		body = getOwner()->getObjectMgr()->getBulletPhysics()->createBox(transform, ext, mass, restitution, friction, linDamping, angDamping);
	}

	void I4ObjectRigidBodyComponent::attachSphere(btScalar radius, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		btTransform transform;
		objectTM2btTransform(transform);
		body = getOwner()->getObjectMgr()->getBulletPhysics()->createSphere(transform, radius, mass, restitution, friction, linDamping, angDamping);
	}

	void I4ObjectRigidBodyComponent::attachCapsule(btScalar radius, btScalar height, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		btTransform transform;
		objectTM2btTransform(transform);
		body = getOwner()->getObjectMgr()->getBulletPhysics()->createCapsule(transform, radius, height, mass, restitution, friction, linDamping, angDamping);
	}

	void I4ObjectRigidBodyComponent::objectTM2btTransform(btTransform& transform)
	{
		I4Matrix4x4 invOffset;
		matOffset.extractInversePrimitive(invOffset);

		I4Matrix4x4 matWorldTM;
		I4Vector3 pos;
		getOwner()->getWorldTM().decompose(nullptr, &matWorldTM, &pos);
		matWorldTM.setTranslation(pos);

		I4Matrix4x4TobtTransform(transform, invOffset*matWorldTM);
	}

	void I4ObjectRigidBodyComponent::btTransform2objectTM(I4Matrix4x4& matWorldTM)
	{
		btTransform worldTM;
		body->getMotionState()->getWorldTransform(worldTM);

		I4Vector3 scale;
		getOwner()->getWorldTM().decompose(&scale, nullptr, nullptr);

		I4Matrix4x4 matScale;
		matScale.makeScale(scale.x, scale.y, scale.z);

		btTransformToI4Matrix4x4(matWorldTM, worldTM);
		matWorldTM = matScale*matOffset*matWorldTM;
	}

}