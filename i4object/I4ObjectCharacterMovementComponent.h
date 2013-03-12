#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4PhysXMgr.h"

using namespace i4core;

namespace i4object
{
	class I4ObjectCharacterMovementComponent : public I4ObjectComponent
	{
	public:
		I4ObjectCharacterMovementComponent();
		virtual ~I4ObjectCharacterMovementComponent(void);

		static const char*	getComponentID()	{ return "Character"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;
		
		void attach(float radius, float height, float slopeLimit, float stepOffset);

		void	setDirection(const I4Vector3& dir);
		void	setMoveSpeed(float speed);
		void	setJumpSpeed(float speed);
		void	setGravity(const I4Vector3& _gravity);

		void	onUpdateLogic(I4MessageArgs& args);

	private:
		PxController*	controller;
		PxVec3			gravity;
		PxVec3			direction;
		float			moveSpeed;
		float			jumpSpeed;
	};
}