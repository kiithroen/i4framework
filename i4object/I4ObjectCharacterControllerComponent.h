#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object
{
	class I4ObjectCharacterControllerComponent : public I4ObjectComponent
	{
	public:
		I4ObjectCharacterControllerComponent(void);
		virtual ~I4ObjectCharacterControllerComponent(void);

		static const char*	getComponentID()	{ return "FlyController"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void activate(bool isActive);

		void onUpdateLogic(I4MessageArgs& args);
	};
}

