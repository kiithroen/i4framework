#include "stdafx.h"
#include "I4ObjectTPSCameraComponent.h"
#include "I4Camera.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Profile.h"

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
		
	}

	void I4ObjectTPSCameraComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
	}

	void I4ObjectTPSCameraComponent::setMainCamera(bool isMain)
	{
		if (isMain)
		{
			getBroadcastMessenger().subscribe(I4Hash("onReadyToRender"), this, bind(&I4ObjectTPSCameraComponent::onReadyToRender, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
		}
	}

	void I4ObjectTPSCameraComponent::onReadyToRender(I4MessageArgs& args)
	{
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

		I4Matrix4x4 matCamView;
		matCamView.makeCameraLookAtLH(position + right*0.3f + up*1.5f + -forward*2.0f, position + up*1.5f, I4VECTOR3_AXISY);


		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matCamView);
	}

}