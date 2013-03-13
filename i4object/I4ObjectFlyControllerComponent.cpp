#include "stdafx.h"
#include "I4ObjectFlyControllerComponent.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Framework.h"

namespace i4object
{

	I4ObjectFlyControllerComponent::I4ObjectFlyControllerComponent(void)
	{
	}


	I4ObjectFlyControllerComponent::~I4ObjectFlyControllerComponent(void)
	{
	}

	void I4ObjectFlyControllerComponent::onAdd()
	{
		prevMouseX = I4InputState::MouseX;
		prevMouseY = I4InputState::MouseY;

		float yawRad;
		float pitchRad;
		float rollRad;

		I4Quaternion rot;
		rot.makeRotationMatrix(getOwner()->getWorldTM());
		rot.extractYawPitchRoll(yawRad, pitchRad, rollRad);

		yaw = I4MathUtil::radianToDegree(yawRad);
		pitch = I4MathUtil::radianToDegree(pitchRad);
	}

	void I4ObjectFlyControllerComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onUpdateLogic"), this);
	}
	
	void I4ObjectFlyControllerComponent::activate(bool isActive)
	{
		if (isActive)
		{
			getBroadcastMessenger().subscribe(I4Hash("onUpdateLogic"), this, bind(&I4ObjectFlyControllerComponent::onUpdateLogic, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onUpdateLogic"), this);
		}
	}

	void I4ObjectFlyControllerComponent::onUpdateLogic(I4MessageArgs& args)
	{
		float dt = args[0].asFloat();

		float moveSpeed = 6.0f*dt;

		I4Vector3 right = getOwner()->getWorldTM().getAxisX();
		I4Vector3 forward = getOwner()->getWorldTM().getAxisZ();
		I4Vector3 position = getOwner()->getPosition();

		right.normalize();
		forward.normalize();

		if (I4InputState::KeyPressed[VK_SHIFT])
		{
			moveSpeed *= 3;
		}

		if (I4InputState::KeyPressed['w'] || I4InputState::KeyPressed['W'])
		{
			position += forward*moveSpeed;
		}

		if (I4InputState::KeyPressed['s'] || I4InputState::KeyPressed['S'])
		{
			position -= forward*moveSpeed;
		}

		if (I4InputState::KeyPressed['a'] || I4InputState::KeyPressed['A'])
		{
			position -= right*moveSpeed;
		}

		if (I4InputState::KeyPressed['d'] || I4InputState::KeyPressed['D'])
		{
			position += right*moveSpeed;
		}

		int curMouseX = I4InputState::MouseX;
		int curMouseY = I4InputState::MouseY;

		if (I4InputState::RightMousePressed)
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

		I4Matrix4x4 mat;
		mat.makeRotationYawPitchRoll(I4MathUtil::degreeToRadian(yaw), I4MathUtil::degreeToRadian(pitch), 0);
		mat.setPosition(position);

		getOwner()->setWorldTM(mat);

		prevMouseX = curMouseX;
		prevMouseY = curMouseY;
	}
}