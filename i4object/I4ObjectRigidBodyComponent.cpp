#include "stdafx.h"
#include "I4ObjectRigidBodyComponent.h"
#include "I4Log.h"
#include "I4Hash.h"
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
	}

	void I4ObjectRigidBodyComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onPreSimulate"), this);
		getBroadcastMessenger().unsubscribe(I4Hash("onPostSimulate"), this);
	}

	void I4ObjectRigidBodyComponent::setOffset(const I4Matrix4x4& m)
	{
		matOffset = m;
	}

	void I4ObjectRigidBodyComponent::setKinematic(bool isKinematic)
	{
		if (isKinematic)
		{
			getBroadcastMessenger().subscribe(I4Hash("onPreSimulate"), this, bind(&I4ObjectRigidBodyComponent::onPreSimulate, this, _1));
			getBroadcastMessenger().unsubscribe(I4Hash("onPostSimulate"), this);

			body->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
		}
		else
		{
			getBroadcastMessenger().subscribe(I4Hash("onPostSimulate"), this, bind(&I4ObjectRigidBodyComponent::onPostSimulate, this, _1));
			getBroadcastMessenger().unsubscribe(I4Hash("onPreSimulate"), this);

			body->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);
		}

	}

	void I4ObjectRigidBodyComponent::onPreSimulate(I4MessageArgs& args)
	{
		I4PROFILE_THISFUNC;

		PxTransform transform;
		WorldTM2PxTransform(transform);
		body->setKinematicTarget(transform);
	}

	void I4ObjectRigidBodyComponent::onPostSimulate(I4MessageArgs& args)
	{
		I4PROFILE_THISFUNC;

		I4Matrix4x4 matWorldTM;
		PxTransform2WorldTM(matWorldTM);
		getOwner()->setLocalTM(matWorldTM);
	}

	void I4ObjectRigidBodyComponent::attachBox(const I4Vector3& ext, float density, bool kinematic)
	{
		PxTransform transform;
		WorldTM2PxTransform(transform);
		body = getOwner()->getObjectMgr()->getPhysXMgr()->createBox(transform, ext, density);

		setKinematic(kinematic);
	}

	void I4ObjectRigidBodyComponent::attachSphere(float radius, float density, bool kinematic)
	{
		PxTransform transform;
		WorldTM2PxTransform(transform);
		body = getOwner()->getObjectMgr()->getPhysXMgr()->createSphere(transform, radius, density);

		setKinematic(kinematic);
	}

	void I4ObjectRigidBodyComponent::attachCapsule(float radius, float height, float density, bool kinematic)
	{
		PxTransform transform;
		WorldTM2PxTransform(transform);
		body = getOwner()->getObjectMgr()->getPhysXMgr()->createCapsule(transform, radius, height, density);

		setKinematic(kinematic);
	}

	void I4ObjectRigidBodyComponent::WorldTM2PxTransform(PxTransform& transform)
	{
		I4Matrix4x4 invOffset;
		matOffset.extractInversePrimitive(invOffset);

		I4Matrix4x4 matWorldTM;
		I4Vector3 pos;
		getOwner()->getWorldTM().decompose(nullptr, &matWorldTM, &pos);
		matWorldTM.setTranslation(pos);

		PxMat44 matTransform;
		convertToPxMat4x4(matTransform, invOffset*matWorldTM);

		transform = PxTransform(matTransform);
	}

	void I4ObjectRigidBodyComponent::PxTransform2WorldTM(I4Matrix4x4& matWorldTM)
	{
		const PxMat44 matTransform = PxMat44(body->getGlobalPose());

		convertToI4Matrix4x4(matWorldTM, matTransform);

		I4Vector3 scale;
		getOwner()->getWorldTM().decompose(&scale, nullptr, nullptr);

		I4Matrix4x4 matScale;
		matScale.makeScale(scale.x, scale.y, scale.z);

		matWorldTM = matScale*matOffset*matWorldTM;
	}

}