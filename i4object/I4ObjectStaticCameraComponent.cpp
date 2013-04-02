#include "stdafx.h"
#include "I4ObjectStaticCameraComponent.h"
#include "I4Camera.h"
#include "I4Hash.h"
#include "I4Renderer.h"
#include "I4Profile.h"

namespace i4object
{

	ObjectStaticCameraComponent::ObjectStaticCameraComponent(void)
	{
	}


	ObjectStaticCameraComponent::~ObjectStaticCameraComponent(void)
	{
	}

	void ObjectStaticCameraComponent::onAdd()
	{
		
	}

	void ObjectStaticCameraComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onLateUpdate"), this);
	}

	void ObjectStaticCameraComponent::setMainCamera(bool isMain)
	{
		if (isMain)
		{
			getBroadcastMessenger().subscribe(Hash("onLateUpdate"), this, bind(&ObjectStaticCameraComponent::onLateUpdate, this, _1));
		}
		else
		{
			getBroadcastMessenger().unsubscribe(Hash("onLateUpdate"), this);
		}
	}

	void ObjectStaticCameraComponent::onLateUpdate(MessageArgs& args)
	{
		Matrix4x4 matTarget = getOwner()->getWorldTM();

		Matrix4x4 matCamView;
		matTarget.extractInverse(matCamView);

		getOwner()->getObjectMgr()->getRenderer()->getMainCamera().setViewMatrix(matCamView);
	}

}