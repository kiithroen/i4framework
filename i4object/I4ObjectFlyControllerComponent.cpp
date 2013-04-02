#include "stdafx.h"
#include "I4ObjectFlyControllerComponent.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Framework.h"

namespace i4object
{

	ObjectFlyControllerComponent::ObjectFlyControllerComponent(void)
	{
	}


	ObjectFlyControllerComponent::~ObjectFlyControllerComponent(void)
	{
	}

	void ObjectFlyControllerComponent::onAdd()
	{
		prevMouseX = InputState::MouseX;
		prevMouseY = InputState::MouseY;

		float yawRad;
		float pitchRad;
		float rollRad;

		Quaternion rot;
		rot.makeRotationMatrix(getOwner()->getWorldTM());
		rot.extractYawPitchRoll(yawRad, pitchRad, rollRad);

		yaw = MathUtil::radianToDegree(yawRad);
		pitch = MathUtil::radianToDegree(pitchRad);
	}

	void ObjectFlyControllerComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onUpdateLogic"), this);
	}
	
	void ObjectFlyControllerComponent::activate(bool isActive)
	{
		if (isActive)
		{
			getBroadcastMessenger().subscribe(Hash("onUpdateLogic"), this, bind(&ObjectFlyControllerComponent::onUpdateLogic, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(Hash("onUpdateLogic"), this);
		}
	}

	void ObjectFlyControllerComponent::onUpdateLogic(MessageArgs& args)
	{
		float dt = args[0].asFloat();

		float moveSpeed = 6.0f*dt;

		Vector3 right = getOwner()->getWorldTM().getAxisX();
		Vector3 forward = getOwner()->getWorldTM().getAxisZ();
		Vector3 position = getOwner()->getPosition();

		right.normalize();
		forward.normalize();

		if (InputState::KeyPressed[VK_SHIFT])
		{
			moveSpeed *= 3;
		}

		if (InputState::KeyPressed['w'] || InputState::KeyPressed['W'])
		{
			position += forward*moveSpeed;
		}

		if (InputState::KeyPressed['s'] || InputState::KeyPressed['S'])
		{
			position -= forward*moveSpeed;
		}

		if (InputState::KeyPressed['a'] || InputState::KeyPressed['A'])
		{
			position -= right*moveSpeed;
		}

		if (InputState::KeyPressed['d'] || InputState::KeyPressed['D'])
		{
			position += right*moveSpeed;
		}

		int curMouseX = InputState::MouseX;
		int curMouseY = InputState::MouseY;

		if (InputState::RightMousePressed)
		{
			int dx = curMouseX - prevMouseX;
			int dy = curMouseY - prevMouseY;

			const float CAMERA_SENSITIVE = 0.35f;
			yaw += dx*CAMERA_SENSITIVE;
			pitch += dy*CAMERA_SENSITIVE;

			if (yaw > 360)
			{
				yaw -= 360;
			}

			if (pitch < -90)
			{
				pitch = -90;
			}

			if (pitch > 90)
			{
				pitch = 90;
			}		
		}

		Matrix4x4 mat;
		mat.makeRotationYawPitchRoll(MathUtil::degreeToRadian(yaw), MathUtil::degreeToRadian(pitch), 0);
		mat.setPosition(position);

		getOwner()->setWorldTM(mat);

		prevMouseX = curMouseX;
		prevMouseY = curMouseY;
	}
}