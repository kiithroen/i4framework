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
	class I4ObjectViewComponent : public I4ObjectComponent
	{
	public:
		I4ObjectViewComponent(void);
		virtual ~I4ObjectViewComponent(void);

		static const char*		getComponentID()	{ return "View"; }

		virtual void			onAdd() override;
		virtual void			onRemove() override;

		bool attachModel(const char* name, const char* modelPrefixName, bool hasMesh, bool hasMtrl, bool hasBone);
		bool attachAni(const char* fname, const char* aniName);
		
		void playAnimation(const char* name);

		void onUpateAnimation(I4MessageArgs& args);
		void onCommitToRenderer(I4MessageArgs& args);

		void setOffset(const I4Matrix4x4& _offset)		{ offset = _offset; }
		I4Model* getModel()								{ return model; }

		void setShadowCaster(bool enable);
		void setShadowReceiver(bool enable);
	private:
		I4Model*		model;
		I4Matrix4x4		offset;
	};
}