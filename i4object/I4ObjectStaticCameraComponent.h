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

	class I4ObjectStaticCameraComponent : public I4ObjectComponent
	{
	public:
		I4ObjectStaticCameraComponent(void);
		virtual ~I4ObjectStaticCameraComponent(void);

		static const char*	getComponentID()	{ return "StaticCamera"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;

		void setMainCamera(bool isMain);
		void setOffset(const I4Vector3& _offset)		{ offset = _offset; }

		void onReadyToRender(I4MessageArgs& args);

	private:
		I4Vector3	offset;
		bool		isMainCamera;
	};

}
