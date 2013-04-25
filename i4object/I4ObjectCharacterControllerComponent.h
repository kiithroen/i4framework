#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object
{
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
		bool	isMoving;
		bool	isJumping;
	};
}

