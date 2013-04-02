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

	class ObjectTPSCameraComponent : public ObjectComponent
	{
	public:
		ObjectTPSCameraComponent(void);
		virtual ~ObjectTPSCameraComponent(void);

		static const char*	getComponentID()	{ return "TPSCamera"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void setMainCamera(bool isMain);

		void onLateUpdate(MessageArgs& args);

	private:
		bool		isMainCamera;

		int			prevMouseX;
		int			prevMouseY;

		float		yaw;
		float		pitch;

		Vector3	originalEyeDir;
		Vector3	eyeDir;
	};

}
