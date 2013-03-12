#include "stdafx.h"
#include "I4ObjectTPSCameraComponent.h"
#include "I4Camera.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Profile.h"
#include "I4Framework.h"

namespace i4object
{

	I4ObjectTPSCameraComponent::I4ObjectTPSCameraComponent(void)
	{
	}


	I4ObjectTPSCameraComponent::~I4ObjectTPSCameraComponent(void)
	{
	}

	void I4ObjectTPSCameraComponent::onAdd()
	{
		prevMouseX = I4InputState::MouseX;
		prevMouseY = I4InputState::MouseY;

		float yawRad;
		float pitchRad;
		float rollRad;

		I4Quaternion rot;
		rot.makeRotationMatrix(getOwner()->getLocalTM());
		rot.extractYawPitchRoll(yawRad, pitchRad, rollRad);

		yaw = I4MathUtil::radianToDegree(yawRad);
		pitch = I4MathUtil::radianToDegree(pitchRad);

		getOwner()->getLocalTM().extractAxisZ(originalEyeDir);
		originalEyeDir.normalize();

		eyeDir = originalEyeDir;
	}

	void I4ObjectTPSCameraComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onLateUpdate"), this);
	}

	void I4ObjectTPSCameraComponent::setMainCamera(bool isMain)
	{
		if (isMain)
		{
			getBroadcastMessenger().subscribe(I4Hash("onLateUpdate"), this, bind(&I4ObjectTPSCameraComponent::onLateUpdate, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onLateUpdate"), this);
		}
	}

	void I4ObjectTPSCameraComponent::onLateUpdate(I4MessageArgs& args)
	{
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

			if (pitch < 0)
			{
				pitch = 0;
			}

			if (pitch > 60)
			{
				pitch = 60;
			}

			I4Quaternion q;
			q.makeRotationYawPitchRoll(I4MathUtil::degreeToRadian(yaw), I4MathUtil::degreeToRadian(pitch), 0);

			eyeDir = q.transform(originalEyeDir);
		}		
		
		prevMouseX = curMouseX;
		prevMouseY = curMouseY;

		I4Matrix4x4 matTarget = getOwner()->getWorldTM();

		I4Vector3 forward;
		matTarget.extractAxisZ(forward);
		forward.normalize();

		I4Vector3 up;
		matTarget.extractAxisY(up);
		up.normalize();

		I4Vector3 right;
		matTarget.extractAxisX(right);
		right.normalize();

		I4Vector3 position;
		matTarget.extractTranslation(position);

		I4Vector3 eye = position - eyeDir*5.0f + right*0.2f + up*1.5f;
		I4Matrix4x4 matCamView;
		matCamView.makeCameraLookAtLH(eye, eye + eyeDir, I4VECTOR3_AXISY);

		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matCamView);
	}

}