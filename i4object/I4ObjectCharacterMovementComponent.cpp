#include "stdafx.h"
#include "I4ObjectCharacterMovementComponent.h"
#include "I4Log.h"
#include "I4Hash.h"
#include "I4Profile.h"

namespace i4object {

	I4ObjectCharacterMovementComponent::I4ObjectCharacterMovementComponent(void)
		: controller(nullptr)
	{
	}


	I4ObjectCharacterMovementComponent::~I4ObjectCharacterMovementComponent(void)
	{
	}

	void I4ObjectCharacterMovementComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onUpdateLogic"), this, bind(&I4ObjectCharacterMovementComponent::onUpdateLogic, this, _1));

		setGravity(I4Vector3(0, -9.8f, 0));

		setDirection(I4VECTOR3_AXISZ);
		setMoveSpeed(0);
	}

	void I4ObjectCharacterMovementComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onUpdateLogic"), this);
	}

	void I4ObjectCharacterMovementComponent::attach(float radius, float height, float slopeLimit, float stepOffset)
	{
		I4Vector3 p = getOwner()->getPosition();
		controller = getOwner()->getObjectMgr()->getPhysXMgr()->createCapsuleController(p, radius, height, slopeLimit, stepOffset, nullptr, nullptr);
	}

	void I4ObjectCharacterMovementComponent::onUpdateLogic(I4MessageArgs& args)
	{
		I4PROFILE_THISFUNC;

		float dt = args[0].asFloat();

		PxVec3 velocity = direction*moveSpeed;
		velocity.y += jumpSpeed;

		PxVec3 disp = velocity*dt;
		disp += gravity*dt;

		const PxU32 flag = controller->move(disp, 0.001f, dt, PxControllerFilters());

		const PxExtendedVec3 p = controller->getFootPosition();
		getOwner()->setPosition(I4Vector3((float)p.x, (float)p.y, (float)p.z));
	}

	void I4ObjectCharacterMovementComponent::setDirection(const I4Vector3& dir)
	{
		convertToPxVec3(direction, dir);
	}

	void I4ObjectCharacterMovementComponent::setMoveSpeed(float speed)
	{
		moveSpeed = speed;
	}

	void I4ObjectCharacterMovementComponent::setJumpSpeed(float speed)
	{
		jumpSpeed = speed;
	}

	void I4ObjectCharacterMovementComponent::setGravity(const I4Vector3& _gravity)
	{
		convertToPxVec3(gravity, _gravity);
	}
}