#include "stdafx.h"
#include "I4ObjectTPSCameraComponent.h"
#include "I4Camera.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Profile.h"
#include "I4Framework.h"

namespace i4object
{

	ObjectTPSCameraComponent::ObjectTPSCameraComponent(void)
	{
	}


	ObjectTPSCameraComponent::~ObjectTPSCameraComponent(void)
	{
	}

	void ObjectTPSCameraComponent::onAdd()
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

		originalEyeDir = getOwner()->getWorldTM().getAxisZ();
		originalEyeDir.normalize();

		eyeDir = originalEyeDir;
	}

	void ObjectTPSCameraComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onLateUpdate"), this);
	}

	void ObjectTPSCameraComponent::setMainCamera(bool isMain)
	{
		if (isMain)
		{
			getBroadcastMessenger().subscribe(Hash("onLateUpdate"), this, bind(&ObjectTPSCameraComponent::onLateUpdate, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(Hash("onLateUpdate"), this);
		}
	}

	void ObjectTPSCameraComponent::onLateUpdate(MessageArgs& args)
	{
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

			if (pitch < -30)
			{
				pitch = -30;
			}

			if (pitch > 60)
			{
				pitch = 60;
			}

			Quaternion q;
			q.makeRotationYawPitchRoll(MathUtil::degreeToRadian(yaw), MathUtil::degreeToRadian(pitch), 0);

			eyeDir = q.transform(originalEyeDir);
		}		
		
		prevMouseX = curMouseX;
		prevMouseY = curMouseY;

		Matrix4x4 matTarget = getOwner()->getWorldTM();
		
		Vector3 up = matTarget.getAxisY();
		up.normalize();

		Vector3 right = getOwner()->getObjectMgr()->getRenderer()->getMainCamera().getWorldMatrix().getAxisX();
		right.normalize();

		Vector3 position = matTarget.getPosition();

		Vector3 eye = position - eyeDir*1.8f + right*0.25f + up*0.95f;
		Matrix4x4 matCamView;
		matCamView.makeCameraLookAtLH(eye, eye + eyeDir, VECTOR3_AXISY);

		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matCamView);
	}

}