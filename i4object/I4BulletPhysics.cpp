#include "stdafx.h"
#include "I4BulletPhysics.h"
#include "I4Log.h"
#include "I4Profile.h"
using namespace i4core;

namespace i4object
{
	I4BulletPhysics::I4BulletPhysics(void)
	: dynamicsWorld(nullptr)
	, solver(nullptr)
	, overlappingPairCache(nullptr)
	, dispatcher(nullptr)
	, collisionConfiguration(nullptr)
	{
	}

	I4BulletPhysics::~I4BulletPhysics()
	{
		destroy();
	}

	bool I4BulletPhysics::init()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache , solver , collisionConfiguration );
		dynamicsWorld -> setGravity ( btVector3 (0 , -10 ,0));

		return true;
	}

	void I4BulletPhysics::destroy()
	{
		for (int i = dynamicsWorld->getNumCollisionObjects()-1; i >=0 ; i --)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}
		
		for (int j = 0; j < collisionShapes.size (); j ++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		delete dynamicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;

		collisionShapes.clear();
	}	

	void I4BulletPhysics::simulate(float dt)
	{
		I4PROFILE_THISFUNC;

		dynamicsWorld->stepSimulation(dt);
	}

	btRigidBody* I4BulletPhysics::createBox(const btTransform& bodyTM, const btVector3& ext, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		return createRigidBody(new btBoxShape(ext), bodyTM, mass, restitution, friction, linDamping, angDamping);

	}

	btRigidBody* I4BulletPhysics::createSphere(const btTransform& bodyTM, btScalar radius, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		return createRigidBody(new btSphereShape(radius), bodyTM, mass, restitution, friction, linDamping, angDamping);
	}

	btRigidBody* I4BulletPhysics::createCapsule(const btTransform& bodyTM, btScalar radius, btScalar height, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		return createRigidBody(new btCapsuleShape(radius, height), bodyTM, mass, restitution, friction, linDamping, angDamping);
	}

	btRigidBody* I4BulletPhysics::createRigidBody(btCollisionShape* shape, const btTransform& bodyTM, btScalar mass, btScalar restitution, btScalar friction, btScalar linDamping, btScalar angDamping)
	{
		collisionShapes.push_back(shape);

		btVector3 localInertia(0,0,0);
		if (mass != 0)
		{
			shape->calculateLocalInertia(mass, localInertia);
		}

		btDefaultMotionState* myMotionState = new btDefaultMotionState(bodyTM);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);		
		rbInfo.m_restitution = restitution;
		rbInfo.m_friction = friction;
		rbInfo.m_linearDamping = linDamping;
		rbInfo.m_angularDamping = angDamping;

		btRigidBody* body = new btRigidBody(rbInfo);
		dynamicsWorld->addRigidBody(body);

		return body;
	}

}
