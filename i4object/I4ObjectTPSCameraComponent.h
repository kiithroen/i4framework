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

	class I4ObjectTPSCameraComponent : public I4ObjectComponent
	{
	public:
		I4ObjectTPSCameraComponent(void);
		virtual ~I4ObjectTPSCameraComponent(void);

		static const char*	getComponentID()	{ return "TPSCamera"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void setMainCamera(bool isMain);

		void onReadyToRender(I4MessageArgs& args);

	private:
		bool		isMainCamera;
	};

}
