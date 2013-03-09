#include "stdafx.h"
#include "I4ObjectFlyControllerComponent.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Framework.h"

namespace i4object
{

	I4ObjectFlyControllerComponent::I4ObjectFlyControllerComponent(void)
		: eyeHeight(1.5f)
	{
	}


	I4ObjectFlyControllerComponent::~I4ObjectFlyControllerComponent(void)
	{
	}

	void I4ObjectFlyControllerComponent::onAdd()
	{
		prevMouseX = I4InputState::mouseX;
		prevMouseY = I4InputState::mouseY;

		float yawRad;
		float pitchRad;
		float rollRad;

		I4Quaternion rot;
		rot.makeRotationMatrix(getOwner()->getLocalTM());
		rot.extractYawPitchRoll(yawRad, pitchRad, rollRad);

		yaw = I4MathUtil::radianToDegree(yawRad);
		pitch = I4MathUtil::radianToDegree(pitchRad);
		roll = I4MathUtil::radianToDegree(rollRad);
	}

	void I4ObjectFlyControllerComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onPostUpdate"), this);
	}
	
	void I4ObjectFlyControllerComponent::activate(bool isActive)
	{
		if (isActive)
		{
			getBroadcastMessenger().subscribe(I4Hash("onPostUpdate"), this, bind(&I4ObjectFlyControllerComponent::onPostUpdate, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onPostUpdate"), this);
		}
	}

	void I4ObjectFlyControllerComponent::onPostUpdate(I4MessageArgs& args)
	{
		float moveSpeed = 6.0f*args[0].asFloat();

		getOwner()->getLocalTM().extractAxisX(right);
		getOwner()->getLocalTM().extractAxisZ(direction);
		getOwner()->getLocalTM().extractTranslation(position);

		if (I4InputState::KeyPressed['w'] || I4InputState::KeyPressed['W'])
		{
			position += direction*moveSpeed;
		}

		if (I4InputState::KeyPressed['s'] || I4InputState::KeyPressed['S'])
		{
			position -= direction*moveSpeed;
		}

		if (I4InputState::KeyPressed['a'] || I4InputState::KeyPressed['A'])
		{
			position -= right*moveSpeed;
		}

		if (I4InputState::KeyPressed['d'] || I4InputState::KeyPressed['D'])
		{
			position += right*moveSpeed;
		}

		int curMouseX = I4InputState::mouseX;
		int curMouseY = I4InputState::mouseY;

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
		mat.makeRotationYawPitchRoll(I4MathUtil::degreeToRadian(yaw), I4MathUtil::degreeToRadian(pitch), I4MathUtil::degreeToRadian(roll));
		mat.setTranslation(position);

		getOwner()->setLocalTM(mat);

		prevMouseX = curMouseX;
		prevMouseY = curMouseY;
	}
}