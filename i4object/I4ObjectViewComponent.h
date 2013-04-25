#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4graphics 
{
	class Model;
}

namespace i4object
{
	class ObjectViewComponent : public ObjectComponent
	{
	public:
		ObjectViewComponent(void);
		virtual ~ObjectViewComponent(void);

		static const char*		getComponentID()	{ return "View"; }

		virtual void			onAdd() override;
		virtual void			onRemove() override;

		bool attachModel(const char* name, const char* modelPrefixName, bool hasMesh, bool hasMtrl, bool hasBone);
		bool attachAni(const char* fname, const char* aniName, float start, float end);
		
		void playAnimation(const char* name, float speed = 1.0f);

		void onUpateAnimation(MessageArgs& args);
		void onCommitToRenderer(MessageArgs& args);

		void setOffset(const Matrix4x4& _offset)		{ offset = _offset; }
		Model* getModel()								{ return model; }

		void setShadowCaster(bool enable);
		void setShadowReceiver(bool enable);
	private:
		Model*		model;
		Matrix4x4	offset;
	};
}