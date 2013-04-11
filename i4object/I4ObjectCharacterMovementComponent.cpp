#include "stdafx.h"
#include "I4ObjectCharacterMovementComponent.h"
#include "I4Log.h"
#include "I4Hash.h"
#include "I4Profile.h"

namespace i4object {

	static class CharacterHitReport : public PxUserControllerHitReport
	{
		virtual void onShapeHit(const PxControllerShapeHit& hit)
		{
			if (hit.dir.dot(PxVec3(0, 1, 0)) < -0.5f)
				return;

			PxRigidDynamic* rigid = hit.shape->getActor().is<PxRigidDynamic>();
			if (rigid && !(rigid->getRigidDynamicFlags() & PxRigidDynamicFlag::eKINEMATIC))
			{
				PxRigidBodyExt::addForceAtPos(*rigid, hit.dir*3, PxVec3((PxReal)hit.worldPos.x, (PxReal)hit.worldPos.y, (PxReal)hit.worldPos.z));
			}
		}

		virtual void onControllerHit(const PxControllersHit& hit)
		{
		}

		virtual void onObstacleHit(const PxControllerObstacleHit& hit)
		{
		}
	} charHitReport;


	ObjectCharacterMovementComponent::ObjectCharacterMovementComponent(void)
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


	ObjectCharacterMovementComponent::~ObjectCharacterMovementComponent(void)
	{
	}

	void ObjectCharacterMovementComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(Hash("onUpdateLogic"), this, bind(&ObjectCharacterMovementComponent::onUpdateLogic, this, _1));
		setDirection(VECTOR3_AXISZ);
	}

	void ObjectCharacterMovementComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onUpdateLogic"), this);
	}

	void ObjectCharacterMovementComponent::attach(float radius, float height, float slopeLimit, float stepOffset)
	{
		Vector3 p = getOwner()->getPosition();
		controller = getOwner()->getObjectMgr()->getPhysXMgr()->createCapsuleController(p, radius, height, slopeLimit, stepOffset, &charHitReport, nullptr);
	}

	void ObjectCharacterMovementComponent::onUpdateLogic(MessageArgs& args)
	{
		PROFILE_THISFUNC;

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
		getOwner()->setPosition(Vector3((float)p.x, (float)p.y, (float)p.z));
	}

	void ObjectCharacterMovementComponent::setDirection(const Vector3& dir)
	{
		if (grounded == false)
			return;

		convertTo(direction, dir);
	}

	void ObjectCharacterMovementComponent::move(float speed)
	{
		if (grounded == false)
			return;

		moveSpeed = speed;
		stopAccel = 0.0f;
		moving = true;
		stopped = false;
	}

	void ObjectCharacterMovementComponent::stop()
	{
		if (stopped)
			return;

		stopAccel = -40;
		stopped = true;
	}

	void ObjectCharacterMovementComponent::jump(float speed)
	{
		if (grounded == false)
			return;

		jumpSpeed = speed;

		grounded = false;
	}

	void ObjectCharacterMovementComponent::setGravity(float _gravity)
	{
		gravity = _gravity;
	}
}