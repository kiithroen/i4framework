#include "stdafx.h"
#include "I4ObjectStaticCameraComponent.h"
#include "I4Camera.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Profile.h"

namespace i4object
{

	I4ObjectStaticCameraComponent::I4ObjectStaticCameraComponent(void)
		: isActivate(false)
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

	void I4ObjectStaticCameraComponent::activate(bool isActive)
	{
		if (isActive)
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
		I4Matrix4x4 matView;
		getOwner()->getWorldTM().extractInversePrimitive(matView);
		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matView);
	}

}