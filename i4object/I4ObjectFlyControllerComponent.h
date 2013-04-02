#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object
{
	class ObjectFlyControllerComponent : public ObjectComponent
	{
	public:
		ObjectFlyControllerComponent(void);
		virtual ~ObjectFlyControllerComponent(void);

		static const char*	getComponentID()	{ return "FlyController"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void activate(bool isActive);

		void onUpdateLogic(MessageArgs& args);

	private:
		int					prevMouseX;
		int					prevMouseY;
		
		float				yaw;
		float				pitch;
	};
}
