#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object
{
	enum OBJECT_STATE
	{
		OBJECT_IDLE = 0,
		OBJECT_RUNNING,
		OBJECT_WALKING,
		OBJECT_JUMPING,
		OBJECT_FALLING,
	};

	class ObjectCharacterControllerComponent : public ObjectComponent
	{
	public:
		ObjectCharacterControllerComponent(void);
		virtual ~ObjectCharacterControllerComponent(void);

		static const char*	getComponentID()	{ return "FlyController"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void activate(bool isActive);

		void onUpdateLogic(MessageArgs& args);

	private:
		OBJECT_STATE	state;
	};
}

