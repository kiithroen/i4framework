#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object
{
	class I4ObjectFlyControllerComponent : public I4ObjectComponent
	{
	public:
		I4ObjectFlyControllerComponent(void);
		virtual ~I4ObjectFlyControllerComponent(void);

		static const char*	getComponentID()	{ return "FlyController"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void activate(bool isActive);

		void onPostUpdate(I4MessageArgs& args);

	private:
		I4Vector3			right;
		I4Vector3			direction;
		I4Vector3			position;

		int					prevMouseX;
		int					prevMouseY;
		
		float				yaw;
		float				pitch;
		float				roll;

		float				eyeHeight;
	};
}
