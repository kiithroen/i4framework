#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4core
{
	class Camera;
}

namespace i4object
{

	class ObjectStaticCameraComponent : public ObjectComponent
	{
	public:
		ObjectStaticCameraComponent(void);
		virtual ~ObjectStaticCameraComponent(void);

		static const char*	getComponentID()	{ return "StaticCamera"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void setMainCamera(bool isMain);

		void onLateUpdate(MessageArgs& args);

	private:
		bool		isMainCamera;
	};

}
