#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4core
{
	class I4Camera;
}

namespace i4object
{

	class I4ObjectCameraComponent : public I4ObjectComponent
	{
	public:
		I4ObjectCameraComponent(void);
		virtual ~I4ObjectCameraComponent(void);

		static const char*	getComponentID()	{ return "Camera"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void makeMainCamera(bool val);

		void onReadyToRender(I4MessageArgs& args);

	private:
		bool	isActivate;
	};

}
