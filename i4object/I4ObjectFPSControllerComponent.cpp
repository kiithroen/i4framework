#include "stdafx.h"
#include "I4ObjectFPSControllerComponent.h"
#include "I4Hash.h"
#include "I4Framework.h"

namespace i4object
{

	I4ObjectFPSControllerComponent::I4ObjectFPSControllerComponent(void)
	{
	}


	I4ObjectFPSControllerComponent::~I4ObjectFPSControllerComponent(void)
	{
	}

	void I4ObjectFPSControllerComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onPostUpdate"), this, bind(&I4ObjectFPSControllerComponent::onPostUpdate, this, _1));

		prevMouseX = I4InputState::mouseX;
		prevMouseY = I4InputState::mouseY;

		float camYawRad;
		float camPitchRad;
		float camRollRad;

		I4Quaternion camRotation;
		camRotation.makeRotationMatrix(getOwner()->getLocalTM());
		camRotation.extractYawPitchRoll(camYawRad, camPitchRad, camRollRad);

		camYaw = I4MathUtil::radianToDegree(camYawRad);
		camPitch = I4MathUtil::radianToDegree(camPitchRad);
		camRoll = I4MathUtil::radianToDegree(camRollRad);
	}

	void I4ObjectFPSControllerComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onPostUpdate"), this);
	}

	void I4ObjectFPSControllerComponent::onPostUpdate(I4MessageArgs& args)
	{
		float camMoveSpeed = 6.0f*args[0].asFloat();

		I4Vector3 camRight;
		I4Vector3 camDirection;
		I4Vector3 newCamEye;

		getOwner()->getLocalTM().extractAxisX(camRight);
		getOwner()->getLocalTM().extractAxisZ(camDirection);
		getOwner()->getLocalTM().extractTranslation(newCamEye);

		if (I4InputState::KeyPressed['w'] || I4InputState::KeyPressed['W'])
		{
			newCamEye += camDirection*camMoveSpeed;
		}

		if (I4InputState::KeyPressed['s'] || I4InputState::KeyPressed['S'])
		{
			newCamEye -= camDirection*camMoveSpeed;
		}

		if (I4InputState::KeyPressed['a'] || I4InputState::KeyPressed['A'])
		{
			newCamEye -= camRight*camMoveSpeed;
		}

		if (I4InputState::KeyPressed['d'] || I4InputState::KeyPressed['D'])
		{
			newCamEye += camRight*camMoveSpeed;
		}

		int curMouseX = I4InputState::mouseX;
		int curMouseY = I4InputState::mouseY;

		if (I4InputState::RightMousePressed)
		{
			int dx = curMouseX - prevMouseX;
			int dy = curMouseY - prevMouseY;

			const float CAMERA_SENSITIVE = 0.35f;
			camYaw += dx*CAMERA_SENSITIVE;
			camPitch += dy*CAMERA_SENSITIVE;

			if (camYaw > 360)
			{
				camYaw -= 360;
			}

			if (camPitch < -90)
			{
				camPitch = -90;
			}

			if (camPitch > 90)
			{
				camPitch = 90;
			}		
		}

		I4Matrix4x4 matCam;
		matCam.makeRotationYawPitchRoll(I4MathUtil::degreeToRadian(camYaw), I4MathUtil::degreeToRadian(camPitch), I4MathUtil::degreeToRadian(camRoll));
		matCam.setTranslation(newCamEye);

		getOwner()->setLocalTM(matCam);

		prevMouseX = curMouseX;
		prevMouseY = curMouseY;
	}

}