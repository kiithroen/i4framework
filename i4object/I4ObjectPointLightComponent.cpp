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
		getBroadcastMessenger().subscribe(I4Hash("onUpdateLogic"), this, bind(&I4ObjectPointLightComponent::onUpdateLogic, this, _1));
		getBroadcastMessenger().subscribe(I4Hash("onCommitToRenderer"), this, bind(&I4ObjectPointLightComponent::onCommitToRenderer, this, _1));
	}

	void I4ObjectPointLightComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onUpdateLogic"), this);
		getBroadcastMessenger().unsubscribe(I4Hash("onCommitToRenderer"), this);
	}


	void I4ObjectPointLightComponent::onUpdateLogic(I4MessageArgs& args)
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
	
	void I4ObjectPointLightComponent::onCommitToRenderer(I4MessageArgs& args)
	{
		if (isTurnOn)
		{
			light.position = getOwner()->getPosition();
			getOwner()->getObjectMgr()->getRenderer()->commit(&light);
		}
	}

}