#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4BulletPhysics.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

using namespace i4core;

namespace i4object
{
	class I4ObjectCharacterControllerComponent : public I4ObjectComponent
	{
	public:
		I4ObjectCharacterControllerComponent();
		virtual ~I4ObjectCharacterControllerComponent(void);

		static const char*	getComponentID()	{ return "KinematicCharacter"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;
		
		void onPreSimulate(I4MessageArgs& args);
		//void onPostSimulate(I4MessageArgs& args);

	private:
		I4Vector3	direction;
		float		speed;

		btPairCachingGhostObject*	ghostObject;
		btConvexShape*				charShape;
		btVector3					velocity;
		btVector3					gravity;
	};
}