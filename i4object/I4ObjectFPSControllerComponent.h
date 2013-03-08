#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object
{
	class I4ObjectFPSControllerComponent : public I4ObjectComponent
	{
	public:
		I4ObjectFPSControllerComponent(void);
		virtual ~I4ObjectFPSControllerComponent(void);

		static const char*	getComponentID()	{ return "FPSController"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void onPostUpdate(I4MessageArgs& args);

	private:
		int					prevMouseX;
		int					prevMouseY;

		float				camYaw;
		float				camPitch;
		float				camRoll;
	};
}
