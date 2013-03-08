#include "stdafx.h"
#include "I4ObjectCameraComponent.h"
#include "I4Camera.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Profile.h"

namespace i4object
{

	I4ObjectCameraComponent::I4ObjectCameraComponent(void)
		: isActivate(false)
	{
	}


	I4ObjectCameraComponent::~I4ObjectCameraComponent(void)
	{
	}

	void I4ObjectCameraComponent::onAdd()
	{
		
	}

	void I4ObjectCameraComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
	}

	void I4ObjectCameraComponent::makeMainCamera(bool isMainCam)
	{
		if (isMainCam)
		{
			getBroadcastMessenger().subscribe(I4Hash("onReadyToRender"), this, bind(&I4ObjectCameraComponent::onReadyToRender, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(I4Hash("onReadyToRender"), this);
		}
	}

	void I4ObjectCameraComponent::onReadyToRender(I4MessageArgs& args)
	{
		I4Matrix4x4 matView;
		getOwner()->getWorldTM().extractInversePrimitive(matView);
		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matView);
	}

}