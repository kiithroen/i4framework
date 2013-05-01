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
		: state(OBJECT_IDLE)
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
		{
			
			if (movement->isFalling())
			{
				if (state != OBJECT_FALLING)
				{
					view->playAnimation("falling");
					state = OBJECT_FALLING;
				}
			}
			return;
		}

		float dt = args[0].asFloat();

		Vector3 camForward = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisZ();
		camForward.y = 0;
		camForward.normalize();

		Vector3 camRight = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		camRight.normalize();

		Vector3 camPos = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		bool isMoveKey = false;

		Vector3 moveDirection = VECTOR3_ZERO;
		if (InputState::KeyPressed['w'] || InputState::KeyPressed['W'])
		{
			moveDirection += camForward;

			isMoveKey = true;
		}

		if (InputState::KeyPressed['s'] || InputState::KeyPressed['S'])
		{
			moveDirection -= camForward;

			isMoveKey = true;
		}

		if (InputState::KeyPressed['a'] || InputState::KeyPressed['A'])
		{
			moveDirection -= camForward*0.1f;
			moveDirection -= camRight;

			isMoveKey = true;
		}

		if (InputState::KeyPressed['d'] || InputState::KeyPressed['D'])
		{
			moveDirection -= camForward*0.1f;
			moveDirection += camRight;

			isMoveKey = true;
		}

		bool isJumpKey = false;
		if (InputState::KeyPressed[VK_SPACE])
		{
			isJumpKey = true;
		}

		if (isJumpKey)
		{
			if (state == OBJECT_IDLE || state == OBJECT_WALKING)
			{
  				movement->jump(4.0f);
				view->playAnimation("jump");
				state = OBJECT_JUMPING;
			}
			else if (state == OBJECT_RUNNING)
			{
				movement->jump(5.0f);
				view->playAnimation("jump");
				state = OBJECT_JUMPING;
			}
		}
		else if (isMoveKey)
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

			if (state == OBJECT_IDLE || state == OBJECT_FALLING)
			{
				if (InputState::KeyPressed[VK_SHIFT])
				{
					movement->move(6);
					view->playAnimation("run");
					state = OBJECT_RUNNING;
				}
				else
				{
					movement->move(2.5f);
					view->playAnimation("walk");
					state = OBJECT_WALKING;
				}
			}
			else if (state == OBJECT_WALKING)
			{
				if (InputState::KeyPressed[VK_SHIFT])
				{
					movement->move(6);
					view->playAnimation("run");
					state = OBJECT_RUNNING;
				}
			}
			else if (state == OBJECT_RUNNING)
			{
				if (InputState::KeyPressed[VK_SHIFT] == false)
				{
					movement->move(2.5f);
					view->playAnimation("walk");
					state = OBJECT_WALKING;
				}
			}
		}
		else
		{
			if (state != OBJECT_IDLE)
			{
				movement->stop();
				view->playAnimation("idle", 0.5f);
				state = OBJECT_IDLE;
			}
		}
	}
}