#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4PhysXMgr.h"

using namespace i4core;

namespace i4object
{
	class I4ObjectCharacterComponent : public I4ObjectComponent
	{
	public:
		I4ObjectCharacterComponent();
		virtual ~I4ObjectCharacterComponent(void);

		static const char*	getComponentID()	{ return "Character"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;
		
		void attach(float radius, float height, float slopeLimit, float stepOffset);

		void onPreSimulate(I4MessageArgs& args);
		void onPostSimulate(I4MessageArgs& args);
	private:
		I4Vector3	direction;
		float		speed;

		PxController*	controller;
		PxVec3			velocity;
		PxVec3			gravity;
	};
}