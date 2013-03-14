#include "stdafx.h"
#include "I4ObjectCharacterMovementComponent.h"
#include "I4Log.h"
#include "I4Hash.h"
#include "I4Profile.h"

namespace i4object {

	I4ObjectCharacterMovementComponent::I4ObjectCharacterMovementComponent(void)
		: controller(nullptr)
		, grounded(false)
		, stopped(false)
		, moving(false)
		, moveSpeed(0)
		, stopAccel(0)
		, jumpSpeed(0)
		, gravity(-9.8f)
	{
	}


	I4ObjectCharacterMovementComponent::~I4ObjectCharacterMovementComponent(void)
	{
	}

	void I4ObjectCharacterMovementComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onUpdateLogic"), this, bind(&I4ObjectCharacterMovementComponent::onUpdateLogic, this, _1));
		setDirection(I4VECTOR3_AXISZ);
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

		PxVec3 deltaMove = PxVec3(0, 0, 0);
		
		if (moving)
		{
			deltaMove += direction*moveSpeed*dt;		// 등속도운도
			
			if (stopped)	// 정지할때는 등속도운동.
			{
				deltaMove += 0.5f*direction*stopAccel*dt*dt;		
			}
		}
		
		if (grounded == false)
		{
			deltaMove.y += jumpSpeed*dt + 0.5f*gravity*dt*dt;
			jumpSpeed += gravity*dt;
		}
		else
		{
			if (stopped)
			{
				moveSpeed += stopAccel*dt;
			}
		}

		if (moveSpeed > 10)
		{
			moveSpeed = 10;
			stopAccel = 0;
		}

		if (moveSpeed < 0)
		{
			moving = false;
		}

		PxVec3 disp = deltaMove;

		const PxU32 flag = controller->move(disp, 0.001f, dt, PxControllerFilters());

		if (flag & PxControllerFlag::eCOLLISION_DOWN)
		{
			grounded = true;
			jumpSpeed = 0;
		}
		else if (flag & PxControllerFlag::eCOLLISION_UP)
		{
			jumpSpeed = 0;
		}
		else
		{
			grounded = false;
		}

		const PxExtendedVec3 p = controller->getFootPosition();
		getOwner()->setPosition(I4Vector3((float)p.x, (float)p.y, (float)p.z));
	}

	void I4ObjectCharacterMovementComponent::setDirection(const I4Vector3& dir)
	{
		if (grounded == false)
			return;

		convertToPxVec3(direction, dir);
	}

	void I4ObjectCharacterMovementComponent::move(float speed)
	{
		if (grounded == false)
			return;

		moveSpeed = speed;
		stopAccel = 0.0f;
		moving = true;
		stopped = false;
	}

	void I4ObjectCharacterMovementComponent::stop()
	{
		if (stopped)
			return;

		stopAccel = -40;
		stopped = true;
	}

	void I4ObjectCharacterMovementComponent::jump(float speed)
	{
		if (grounded == false)
			return;

		jumpSpeed = speed;

		grounded = false;
	}

	void I4ObjectCharacterMovementComponent::setGravity(float _gravity)
	{
		gravity = _gravity;
	}
}