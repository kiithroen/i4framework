#include "stdafx.h"
#include "I4ObjectCharacterControllerComponent.h"
#include "I4ObjectCharacterMovementComponent.h"
#include "I4ObjectViewComponent.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Framework.h"
#include "I4Log.h"

namespace i4object
{

	ObjectCharacterControllerComponent::ObjectCharacterControllerComponent(void)
		: isMoving(false)
		, isJumping(false)
	{
	}


	ObjectCharacterControllerComponent::~ObjectCharacterControllerComponent(void)
	{
	}

	void ObjectCharacterControllerComponent::onAdd()
	{
	}

	void ObjectCharacterControllerComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onUpdateLogic"), this);
	}

	void ObjectCharacterControllerComponent::activate(bool isActive)
	{
		if (isActive)
		{
			getBroadcastMessenger().subscribe(Hash("onUpdateLogic"), this, bind(&ObjectCharacterControllerComponent::onUpdateLogic, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(Hash("onUpdateLogic"), this);
		}
	}

	void ObjectCharacterControllerComponent::onUpdateLogic(MessageArgs& args)
	{
		ObjectCharacterMovementComponent* movement = getOwner()->findComponent<ObjectCharacterMovementComponent>();
		ObjectViewComponent* view = getOwner()->findComponent<ObjectViewComponent>();

		if (movement == nullptr || view == nullptr)
			return;

		if (movement->isGrounded() == false)
			return;

		float dt = args[0].asFloat();

		Vector3 camForward = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisZ();
		camForward.y = 0;
		camForward.normalize();

		Vector3 camRight = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		camRight.normalize();

		Vector3 camPos = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		bool isMove = false;

		Vector3 moveDirection = VECTOR3_ZERO;
		if (InputState::KeyPressed['w'] || InputState::KeyPressed['W'])
		{
			moveDirection += camForward;

			isMove = true;
		}

		if (InputState::KeyPressed['s'] || InputState::KeyPressed['S'])
		{
			moveDirection -= camForward;

			isMove = true;
		}

		if (InputState::KeyPressed['a'] || InputState::KeyPressed['A'])
		{
			moveDirection -= camForward*0.1f;
			moveDirection -= camRight;

			isMove = true;
		}

		if (InputState::KeyPressed['d'] || InputState::KeyPressed['D'])
		{
			moveDirection -= camForward*0.1f;
			moveDirection += camRight;

			isMove = true;
		}

		bool isJump = false;
		if (InputState::KeyPressed[VK_SPACE])
		{
			isJump = true;
		}

		if (isJump)
		{
			if (isJumping == false)
			{
  				movement->jump(6);
				view->playAnimation("jump");
				isJumping = true;
			}
		}
		else if (isMove)
		{
			Vector3 scale = getOwner()->getScale();
			Vector3 position = getOwner()->getPosition();

			Matrix4x4 matScale;
			matScale.makeScale(scale.x, scale.y, scale.z);

			Matrix4x4 matRotPos;
			matRotPos.makeObjectLookAtLH(position, position + moveDirection, VECTOR3_AXISY);

			Quaternion qTarget;
			qTarget.makeRotationMatrix(matRotPos);

			float t = 10*dt;
			if (t > 1)
			{
				t = 1;
			}

			Quaternion qOut;
			Quaternion::slerp(qOut, getOwner()->getRotation(), qTarget, t);

			getOwner()->setRotation(qOut);

			moveDirection.y = 0;
			moveDirection.normalize();
			movement->setDirection(moveDirection);

			if (isMoving == false)
			{				
				movement->move(6);

				view->playAnimation("run");

				isMoving = true;
			}
		}
		else
		{
			if (isMoving)
			{
				movement->stop();
				view->playAnimation("idle");
			}

			isMoving = false;
			isJumping = false;
		}
	}
}