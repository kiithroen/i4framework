#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object {

	class I4OBJECT_API I4ObjectViewComponent : public I4ObjectComponent
	{
	public:
		I4ObjectViewComponent(void);
		~I4ObjectViewComponent(void);

		virtual const char*		getComponentID()	{ return "View"; }

		virtual void			onAdd() override;
		virtual void			onRemove() override;

		void Test(const I4MessageArgs& args);
	};
}