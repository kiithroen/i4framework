#pragma once

#include "I4ObjectComponent.h"
#include "I4Messenger.h"
#include "I4PhysXMgr.h"

using namespace i4core;

namespace i4object
{
	class ObjectCharacterMovementComponent : public ObjectComponent
	{
	public:
		ObjectCharacterMovementComponent();
		virtual ~ObjectCharacterMovementComponent(void);

		static const char*	getComponentID()	{ return "Character"; }

		virtual void		onAdd() override;
		virtual void		onRemove() override;
		
		void attach(float radius, float height, float slopeLimit, float stepOffset);

		void	setDirection(const Vector3& dir);
		void	move(float speed);
		void	jump(float speed);
		void	stop();
		void	setGravity(float gravity);

		bool	isGrounded() const		{ return grounded; }
		bool	isFalling() const		{ return falling; }
		bool	isMoving() const		{ return moving; }
		bool	isStopped() const		{ return stopped; }
		void	onUpdateLogic(MessageArgs& args);

	private:
		PxCapsuleController*	controller;
		PxVec3					direction;
		float					moveSpeed;
		float					stopAccel;
		float					jumpSpeed;
		float					gravity;
		bool					grounded;
		bool					falling;
		bool					moving;
		bool					stopped;
	};
}