#include "stdafx.h"
#include "I4ObjectFPSCameraComponent.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Framework.h"

namespace i4object
{

	I4ObjectFPSCameraComponent::I4ObjectFPSCameraComponent(void)
		: eyeHeight(1.5f)
	{
	}


	I4ObjectFPSCameraComponent::~I4ObjectFPSCameraComponent(void)
	{
	}

	void I4ObjectFPSCameraComponent::onAdd()
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

	void I4ObjectFPSCameraComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onPostUpdate"), this);
		getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
	}
	
	void I4ObjectFPSCameraComponent::activate(bool isActive)
	{
		if (isActive)
		{
			getBroadcastMessenger().subscribe(I4Hash("onPostUpdate"), this, bind(&I4ObjectFPSCameraComponent::onPostUpdate, this, _1));
			getBroadcastMessenger().subscribe(I4Hash("onReadyToRender"), this, bind(&I4ObjectFPSCameraComponent::onReadyToRender, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onPostUpdate"), this);
			getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
		}
	}

	void I4ObjectFPSCameraComponent::onPostUpdate(I4MessageArgs& args)
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
	
	void I4ObjectFPSCameraComponent::onReadyToRender(I4MessageArgs& args)
	{
		I4Matrix4x4 matCamWorld = getOwner()->getWorldTM();
		matCamWorld.setTranslation(I4Vector3(position.x, position.y + eyeHeight, position.z));

		I4Matrix4x4 matCamView;
		matCamWorld.extractInversePrimitive(matCamView);

		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matCamView);
	}
}