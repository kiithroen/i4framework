#include "stdafx.h"
#include "I4ObjectCharacterControllerComponent.h"
#include "I4Log.h"
#include "I4Hash.h"
#include "I4Profile.h"

namespace i4object {

	struct ClosestCallback : public btCollisionWorld::ClosestConvexResultCallback
	{
		ClosestCallback(btCollisionObject* _me)
			: ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
			, me(_me)
		{
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			if (convexResult.m_hitCollisionObject == me)
				return 1.0f;

			return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
		}

		btCollisionObject* me;
	};

	I4ObjectCharacterControllerComponent::I4ObjectCharacterControllerComponent(void)
	{
	}


	I4ObjectCharacterControllerComponent::~I4ObjectCharacterControllerComponent(void)
	{
	}

	void I4ObjectCharacterControllerComponent::onAdd()
	{
		getBroadcastMessenger().subscribe(I4Hash("onPreSimulate"), this, bind(&I4ObjectCharacterControllerComponent::onPreSimulate, this, _1));

		charShape = new btCapsuleShape(0.3f, 1.2f);
		ghostObject = new btPairCachingGhostObject;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(0, 10, 3));
		ghostObject->setWorldTransform(transform);
		ghostObject->setCollisionShape(charShape);

		getOwner()->getObjectMgr()->getBulletPhysics()->getWorld()->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

		velocity = btVector3(0, 0, 0);
		gravity = btVector3(0, -2.0f, 0);
	}

	void I4ObjectCharacterControllerComponent::onRemove()
	{
		getBroadcastMessenger().unsubscribe(I4Hash("onPreSimulate"), this);
	}

	void I4ObjectCharacterControllerComponent::onPreSimulate(I4MessageArgs& args)
	{
		I4PROFILE_THISFUNC;

		ClosestCallback callback(ghostObject);
		callback.m_collisionFilterGroup = btBroadphaseProxy::StaticFilter;

		btTransform transformOld = ghostObject->getWorldTransform();
		btTransform transformNew = transformOld;
		
		btVector3 delta = gravity*(btScalar)args[0].asFloat();
		transformNew.setOrigin(transformNew.getOrigin() + delta);

		ghostObject->convexSweepTest((btConvexShape*)ghostObject->getCollisionShape(), transformOld, transformNew, callback);

		I4LOG_DEBUG << "old : " << transformOld.getOrigin().y();
		I4LOG_DEBUG << "new : " << transformNew.getOrigin().y();
		ClosestCallback cb(ghostObject);
		cb.m_collisionFilterGroup = btBroadphaseProxy::StaticFilter;

//		transformOld.setOrigin(btVector3(0, 10, 3));
//		transformNew.setOrigin(btVector3(0, -23, 3));

		getOwner()->getObjectMgr()->getBulletPhysics()->getWorld()->convexSweepTest((btConvexShape*)ghostObject->getCollisionShape(), transformOld, transformNew, cb);

		if (cb.hasHit())
		{
			I4LOG_DEBUG << "hit1";
		}

		if (callback.hasHit())
		{
			I4LOG_DEBUG << "hit2";
		}
		btVector3 orig = ghostObject->getWorldTransform().getOrigin();
		btVector3 movement = delta*(cb.m_closestHitFraction - 0.01f);
		ghostObject->getWorldTransform().setOrigin(orig + movement);

		I4LOG_DEBUG << "fraction1 : " << callback.m_closestHitFraction;

		I4LOG_DEBUG << "fraction2 : " << cb.m_closestHitFraction;
	}
}