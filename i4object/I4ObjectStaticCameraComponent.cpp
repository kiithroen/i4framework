#include "stdafx.h"
#include "I4ObjectStaticCameraComponent.h"
#include "I4Camera.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Profile.h"

namespace i4object
{

	I4ObjectStaticCameraComponent::I4ObjectStaticCameraComponent(void)
	{
	}


	I4ObjectStaticCameraComponent::~I4ObjectStaticCameraComponent(void)
	{
	}

	void I4ObjectStaticCameraComponent::onAdd()
	{
		
	}

	void I4ObjectStaticCameraComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
	}

	void I4ObjectStaticCameraComponent::setMainCamera(bool isMain)
	{
		if (isMain)
		{
			getBroadcastMessenger().subscribe(I4Hash("onReadyToRender"), this, bind(&I4ObjectStaticCameraComponent::onReadyToRender, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
		}
	}

	void I4ObjectStaticCameraComponent::onReadyToRender(I4MessageArgs& args)
	{
		I4Matrix4x4 matTarget = getOwner()->getWorldTM();

		I4Matrix4x4 matCamView;
		matTarget.extractInversePrimitive(matCamView);

		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matCamView);
	}

}