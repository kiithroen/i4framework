#include "stdafx.h"
#include "I4ObjectPointLightComponent.h"
#include "I4Log.h"
#include "I4ModelMgr.h"
#include "I4Model.h"
#include "I4Renderer.h"

namespace i4object {

	ObjectPointLightComponent::ObjectPointLightComponent(void)
		: isTurnOn(true)
		, blinkEnable(false)
		, blinkTurnOnTime(0)
		, blinkTurnOffTime(0)
		, blinkElapsedTime(0)
	{
		light.color = VECTOR3_ONE;
		light.radius = 1;
		light.position = VECTOR3_ZERO;
	}


	ObjectPointLightComponent::~ObjectPointLightComponent(void)
	{
	}

	void ObjectPointLightComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(Hash("onUpdateLogic"), this, bind(&ObjectPointLightComponent::onUpdateLogic, this, _1));
		getBroadcastMessenger().subscribe(Hash("onCommitToRenderer"), this, bind(&ObjectPointLightComponent::onCommitToRenderer, this, _1));
	}

	void ObjectPointLightComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(Hash("onUpdateLogic"), this);
		getBroadcastMessenger().unsubscribe(Hash("onCommitToRenderer"), this);
	}


	void ObjectPointLightComponent::onUpdateLogic(MessageArgs& args)
	{
		float dt = args[0].asFloat();

		if (blinkEnable == true)
		{
			blinkElapsedTime += dt;

			if (isTurnOn)
			{
				if (blinkElapsedTime >= blinkTurnOnTime)
				{
					blinkElapsedTime = 0;
					isTurnOn = false;
				}
			}
			else
			{
				if (blinkElapsedTime >= blinkTurnOffTime)
				{
					blinkElapsedTime = 0;
					isTurnOn = true;
				}
			}
			
		}
	}
	
	void ObjectPointLightComponent::onCommitToRenderer(MessageArgs& args)
	{
		if (isTurnOn)
		{
			light.position = getOwner()->getPosition();
			getOwner()->getObjectMgr()->getRenderer()->commit(&light);
		}
	}

}