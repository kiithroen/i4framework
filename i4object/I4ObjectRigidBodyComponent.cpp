#include "stdafx.h"
#include "I4ObjectRigidBodyComponent.h"
#include "I4Log.h"
#include "I4Hash.h"
#include "I4Profile.h"

namespace i4object {


	ObjectRigidBodyComponent::ObjectRigidBodyComponent(void)
		: matOffset(MATRIX4X4_IDENTITY)
		, body(nullptr)
	{
	}


	ObjectRigidBodyComponent::~ObjectRigidBodyComponent(void)
	{
	}

	void ObjectRigidBodyComponent::onAdd()
	{
	}

	void ObjectRigidBodyComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onPreSimulate"), this);
		getBroadcastMessenger().unsubscribe(Hash("onPostSimulate"), this);
	}

	void ObjectRigidBodyComponent::setOffset(const Matrix4x4& m)
	{
		matOffset = m;
	}

	void ObjectRigidBodyComponent::setKinematic(bool isKinematic)
	{
		if (isKinematic)
		{
			getBroadcastMessenger().subscribe(Hash("onPreSimulate"), this, bind(&ObjectRigidBodyComponent::onPreSimulate, this, _1));
			getBroadcastMessenger().unsubscribe(Hash("onPostSimulate"), this);

			body->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		}
		else
		{
			getBroadcastMessenger().subscribe(Hash("onPostSimulate"), this, bind(&ObjectRigidBodyComponent::onPostSimulate, this, _1));
			getBroadcastMessenger().unsubscribe(Hash("onPreSimulate"), this);

			body->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);
		}

	}

	void ObjectRigidBodyComponent::onPreSimulate(MessageArgs& args)
	{
		PROFILE_THISFUNC;

		PxTransform transform;
		WorldTM2PxTransform(transform);
		body->setKinematicTarget(transform);
	}

	void ObjectRigidBodyComponent::onPostSimulate(MessageArgs& args)
	{
		PROFILE_THISFUNC;

		Matrix4x4 worldTM;
		PxTransform2WorldTM(worldTM);
		getOwner()->setWorldTM(worldTM);
	}

	void ObjectRigidBodyComponent::attachBox(const Vector3& ext, float density, bool kinematic)
	{
		PxTransform transform;
		WorldTM2PxTransform(transform);
		body = getOwner()->getObjectMgr()->getPhysXMgr()->createBox(transform, ext, density);

		setKinematic(kinematic);
	}

	void ObjectRigidBodyComponent::attachSphere(float radius, float density, bool kinematic)
	{
		PxTransform transform;
		WorldTM2PxTransform(transform);
		body = getOwner()->getObjectMgr()->getPhysXMgr()->createSphere(transform, radius, density);

		setKinematic(kinematic);
	}

	void ObjectRigidBodyComponent::attachCapsule(float radius, float height, float density, bool kinematic)
	{
		PxTransform transform;
		WorldTM2PxTransform(transform);
		body = getOwner()->getObjectMgr()->getPhysXMgr()->createCapsule(transform, radius, height, density);

		setKinematic(kinematic);
	}

	void ObjectRigidBodyComponent::WorldTM2PxTransform(PxTransform& transform)
	{
		Matrix4x4 invOffset;
		matOffset.extractInverse(invOffset);

		Matrix4x4 objectTM;
		Vector3 pos;
		getOwner()->getWorldTM().decompose(nullptr, &objectTM, &pos);
		objectTM.setPosition(pos);

		PxMat44 matTransform;
		convertTo(matTransform, invOffset*objectTM);

		transform = PxTransform(matTransform);
	}

	void ObjectRigidBodyComponent::PxTransform2WorldTM(Matrix4x4& matWorldTM)
	{
		const PxMat44 matTransform = PxMat44(body->getGlobalPose());

		convertTo(matWorldTM, matTransform);

		Vector3 scale;
		getOwner()->getWorldTM().decompose(&scale, nullptr, nullptr);

		Matrix4x4 matScale;
		matScale.makeScale(scale.x, scale.y, scale.z);

		matWorldTM = matScale*matOffset*matWorldTM;
	}

}