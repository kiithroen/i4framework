#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4graphics 
{
	class I4Model;
}

namespace i4object
{
	class I4OBJECT_API I4ObjectViewComponent : public I4ObjectComponent
	{
	public:
		I4ObjectViewComponent(void);
		~I4ObjectViewComponent(void);

		virtual const char*		getComponentID()	{ return "View"; }

		virtual void			onAdd() override;
		virtual void			onRemove() override;

		bool attachModel(const char* name, const char* modelPrefixName, bool hasMesh, bool hasMtrl, bool hasBone);
		bool attachAni(const char* fname, const char* aniName);
		
		void playAnimation(const char* name);

		void onAnimate(I4MessageArgs& args);
		void onRender(I4MessageArgs& args);

	private:
		I4Model*		model;
	};
}