#include "stdafx.h"
#include "I4ObjectCharacterControllerComponent.h"
#include "I4ObjectCharacterMovementComponent.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Framework.h"
#include "I4Log.h"

namespace i4object
{

	I4ObjectCharacterControllerComponent::I4ObjectCharacterControllerComponent(void)
	{
	}


	I4ObjectCharacterControllerComponent::~I4ObjectCharacterControllerComponent(void)
	{
	}

	void I4ObjectCharacterControllerComponent::onAdd()
	{
	}

	void I4ObjectCharacterControllerComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onUpdateLogic"), this);
	}

	void I4ObjectCharacterControllerComponent::activate(bool isActive)
	{
		if (isActive)
		{
			getBroadcastMessenger().subscribe(I4Hash("onUpdateLogic"), this, bind(&I4ObjectCharacterControllerComponent::onUpdateLogic, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onUpdateLogic"), this);
		}
	}

	void I4ObjectCharacterControllerComponent::onUpdateLogic(I4MessageArgs& args)
	{
		I4ObjectCharacterMovementComponent* character = getOwner()->findComponent<I4ObjectCharacterMovementComponent>();
		if (character == nullptr)
			return;

		if (character->isGrounded() == false)
			return;

		float dt = args[0].asFloat();

		I4Vector3 camForward = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisZ();
		camForward.y = 0;
		camForward.normalize();

		I4Vector3 camRight = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		camRight.normalize();

		I4Vector3 camPos = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		bool isMove = false;

		I4Vector3 moveDirection = I4VECTOR3_ZERO;
		if (I4InputState::KeyPressed['w'] || I4InputState::KeyPressed['W'])
		{
			moveDirection += camForward;

			isMove = true;
		}

		if (I4InputState::KeyPressed['s'] || I4InputState::KeyPressed['S'])
		{
			moveDirection -= camForward;

			isMove = true;
		}

		if (I4InputState::KeyPressed['a'] || I4InputState::KeyPressed['A'])
		{
			moveDirection -= camForward*0.1f;
			moveDirection -= camRight;

			isMove = true;
		}

		if (I4InputState::KeyPressed['d'] || I4InputState::KeyPressed['D'])
		{
			moveDirection -= camForward*0.1f;
			moveDirection += camRight;

			isMove = true;
		}

		if (I4InputState::KeyPressed[VK_SPACE])
		{			
  			character->jump(6);
		}

		if (isMove)
		{
			I4Vector3 scale = getOwner()->getScale();
			I4Vector3 position = getOwner()->getPosition();

			I4Matrix4x4 matScale;
			matScale.makeScale(scale.x, scale.y, scale.z);

			I4Matrix4x4 matRotPos;
			matRotPos.makeObjectLookAtLH(position, position + moveDirection, I4VECTOR3_AXISY);

			I4Quaternion qTarget;
			qTarget.makeRotationMatrix(matRotPos);

			float t = 10*dt;
			if (t > 1)
			{
				t = 1;
			}

			I4Quaternion qOut;
			I4Quaternion::slerp(qOut, getOwner()->getRotation(), qTarget, t);

			getOwner()->setRotation(qOut);

			moveDirection.y = 0;
			moveDirection.normalize();
			character->setDirection(moveDirection);
			character->move(6);
		}
		else
		{
			character->stop();
		}
	}
}