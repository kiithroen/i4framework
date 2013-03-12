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

		I4Vector3 scale;
		I4Vector3 pos;
		I4Matrix4x4 mat;
		getOwner()->getLocalTM().decompose(&scale, &mat, &pos);

		I4Vector3 dir;
		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().extractAxisZ(dir);

		dir.y = 0;
		dir.normalize();

		mat.makeObjectLookAtLH(pos, pos + dir, I4VECTOR3_AXISY);

		I4Matrix4x4 matScale;
		matScale.makeScale(scale.x, scale.y, scale.z);

		getOwner()->setLocalTM(matScale*mat);

		I4Vector3			right;
		I4Vector3			forward;

		getOwner()->getLocalTM().extractAxisX(right);
		getOwner()->getLocalTM().extractAxisZ(forward);

		right.normalize();
		forward.normalize();

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
  			character->setJumpSpeed(20);
		}
		else
		{
			character->setJumpSpeed(0);
		}

		if (isMove)
		{
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