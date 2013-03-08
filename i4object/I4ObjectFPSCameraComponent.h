#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object
{
	class I4ObjectFPSCameraComponent : public I4ObjectComponent
	{
	public:
		I4ObjectFPSCameraComponent(void);
		virtual ~I4ObjectFPSCameraComponent(void);

		static const char*	getComponentID()	{ return "FPSController"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void activate(bool isActive);

		void onPostUpdate(I4MessageArgs& args);
		void onReadyToRender(I4MessageArgs& args);

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
