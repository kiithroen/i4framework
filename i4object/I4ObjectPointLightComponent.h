#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4Renderer.h"
using namespace i4core;

namespace i4object
{
	class I4ObjectPointLightComponent : public I4ObjectComponent
	{
	public:
		I4ObjectPointLightComponent(void);
		virtual ~I4ObjectPointLightComponent(void);

		static const char*		getComponentID()	{ return "PointLight"; }

		virtual void			onAdd() override;
		virtual void			onRemove() override;

		void setColor(const I4Vector3& color)
		{
			light.color = color;
		}

		void setRadius(float radius)
		{
			light.radius = radius;
		}

		void enableBlink(float turnOnTime, float turnOffTime)
		{
			blinkEnable = true;
			blinkTurnOnTime = turnOnTime;
			blinkTurnOffTime = turnOffTime;
		}

		void disableBlink()
		{
			blinkEnable = false;
		}

		void turnOn(bool enable)
		{
			isTurnOn = enable;
			if (enable == false)
			{
				disableBlink();
			}
		}

		void onUpdate(I4MessageArgs& args);
		void onRender(I4MessageArgs& args);

	private:
		I4PointLight	light;
		bool			isTurnOn;
		bool			blinkEnable;
		float			blinkTurnOnTime;
		float			blinkTurnOffTime;
		float			blinkElapsedTime;
	};
}