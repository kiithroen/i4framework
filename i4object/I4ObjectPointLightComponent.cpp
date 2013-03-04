#include "stdafx.h"
#include "I4ObjectPointLightComponent.h"
#include "I4Log.h"
#include "I4ModelMgr.h"
#include "I4Model.h"
#include "I4Renderer.h"

namespace i4object {

	I4ObjectPointLightComponent::I4ObjectPointLightComponent(void)
		: isTurnOn(true)
		, blinkEnable(false)
		, blinkTurnOnTime(0)
		, blinkTurnOffTime(0)
		, blinkElapsedTime(0)
	{
		light.color = I4VECTOR3_ONE;
		light.radius = 1;
		light.position = I4VECTOR3_ZERO;
	}


	I4ObjectPointLightComponent::~I4ObjectPointLightComponent(void)
	{
	}

	void I4ObjectPointLightComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onUpdate"), this, bind(&I4ObjectPointLightComponent::onUpdate, this, _1));
		getBroadcastMessenger().subscribe(I4Hash("onRender"), this, bind(&I4ObjectPointLightComponent::onRender, this, _1));
	}

	void I4ObjectPointLightComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onRender"), this);
	}


	void I4ObjectPointLightComponent::onUpdate(I4MessageArgs& args)
	{
		if (blinkEnable == true)
		{
			blinkElapsedTime += args[0].asFloat();

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
	
	void I4ObjectPointLightComponent::onRender(I4MessageArgs& args)
	{
		if (isTurnOn)
		{
			getOwner()->getWorldTM().extractTranslation(light.position);
			getOwner()->getObjectMgr()->getRenderer()->commitToScene(&light);
		}
	}

}