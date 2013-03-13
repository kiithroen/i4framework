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

		float dt = args[0].asFloat();

		I4Vector3 forward = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisZ();
		forward.y = 0;
		forward.normalize();

		I4Vector3 right = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		right.normalize();

		bool isMove = false;
		I4Vector3 direction = I4VECTOR3_ZERO;
		if (I4InputState::KeyPressed['w'] || I4InputState::KeyPressed['W'])
		{
			direction += forward;
			isMove = true;
		}

		if (I4InputState::KeyPressed['s'] || I4InputState::KeyPressed['S'])
		{
			direction -= forward;
			isMove = true;
		}

		if (I4InputState::KeyPressed['a'] || I4InputState::KeyPressed['A'])
		{
			direction -= right;
			isMove = true;
		}

		if (I4InputState::KeyPressed['d'] || I4InputState::KeyPressed['D'])
		{
			direction += right;
			isMove = true;
		}

		if (I4InputState::KeyPressed[VK_SPACE])
		{
  			character->setJumpSpeed(60);
		}
		else
		{
			character->setJumpSpeed(0);
		}

		if (isMove)
		{
			I4Vector3 scale = getOwner()->getScale();
			I4Vector3 position = getOwner()->getPosition();

			I4Matrix4x4 matScale;
			matScale.makeScale(scale.x, scale.y, scale.z);

			I4Matrix4x4 matRotPos;
			matRotPos.makeObjectLookAtLH(position, position + forward, I4VECTOR3_AXISY);
			//matRotPos.setPosition(position);

			I4Quaternion qTarget;
			qTarget.makeRotationMatrix(matRotPos);

			I4Quaternion qOut;
			I4Quaternion::slerp(qOut, getOwner()->getRotation(), qTarget, 10*dt);

			getOwner()->setRotation(qOut);

			direction.y = 0;
			direction.normalize();
			character->setDirection(direction);
			character->setMoveSpeed(10);
		}
		else
		{
			character->setMoveSpeed(0);
		}
	}
}