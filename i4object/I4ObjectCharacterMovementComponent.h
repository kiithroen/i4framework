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
		void	move(float speed);
		void	jump(float speed);
		void	stop();
		void	setGravity(float gravity);

		bool	isGrounded() const		{ return grounded; }
		bool	isMoving() const		{ return moving; }
		bool	isStopped() const		{ return stopped; }
		void	onUpdateLogic(I4MessageArgs& args);

	private:
		PxController*	controller;
		PxVec3			direction;
		float			moveSpeed;
		float			stopAccel;
		float			jumpSpeed;
		float			gravity;
		bool			grounded;
		bool			moving;
		bool			stopped;
	};
}