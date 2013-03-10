#pragma once

#include "btBulletDynamicsCommon.h"
#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics
{
	class I4Renderer;
}
using namespace i4graphics;

namespace i4object
{
	inline void btTransformToI4Matrix4x4(I4Matrix4x4& mat, const btTransform& transform)
	{
		I4Vector3 R = transform.getBasis().getColumn(0);
		I4Vector3 U = transform.getBasis().getColumn(1);
		I4Vector3 L = transform.getBasis().getColumn(2);
		I4Vector3 P = transform.getOrigin();
		
		mat._11 = R.x(); mat._12 = R.y(); mat._13 = R.z(); mat._14 = 0.f;
		mat._21 = U.x(); mat._22 = U.y(); mat._23 = U.z(); mat._24 = 0.f;
		mat._31 = L.x(); mat._32 = L.y(); mat._33 = L.z(); mat._34 = 0.f;
		mat._41 = P.x(); mat._42 = P.y(); mat._43 = P.z(); mat._44 = 1.f;
	}

	inline void I4Matrix4x4TobtTransform(btTransform& transform, const I4Matrix4x4& mat)
	{
		I4Vector3 R,U,L,P;
		R.setX(mat._11); R.setY(mat._12); R.setZ(mat._13);
		U.setX(mat._21); U.setY(mat._22); U.setZ(mat._23);
		L.setX(mat._31); L.setY(mat._32); L.setZ(mat._33);
		P.setX(mat._41); P.setY(mat._42); P.setZ(mat._43);

		transform.getBasis().setValue(R.x(), U.x(), L.x(), 
										R.y(), U.y(), L.y(), 
										R.z(), U.z(), L.z());
		transform.setOrigin(P);
	}

	class I4BulletDebugDraw;

	class I4BulletPhysics
	{
	public:
		I4BulletPhysics(void);
		~I4BulletPhysics(void);

		bool init(I4Renderer* renderer);
		void destroy();

		void simulate(float dt);
		void debugDraw();

		btRigidBody* createBox(const btTransform& bodyTM, const I4Vector3& ext, float mass, float restitution, float friction, float linDamping, float angDamping);
		btRigidBody* createSphere(const btTransform& bodyTM, float radius, float mass, float restitution, float friction, float linDamping, float angDamping);
		btRigidBody* createCapsule(const btTransform& bodyTM, float radius, float height, float mass, float restitution, float friction, float linDamping, float angDamping);

		btDiscreteDynamicsWorld*	getWorld()		{ return dynamicsWorld; }

	private:
		btRigidBody* createRigidBody(btCollisionShape* shape, const btTransform& bodyTM, float mass, float restitution, float friction, float linDamping, float angDamping);


	private:
		btDefaultCollisionConfiguration*		collisionConfiguration;
		btCollisionDispatcher*					dispatcher;
		btBroadphaseInterface*					overlappingPairCache;
		btSequentialImpulseConstraintSolver*	solver;
		btDiscreteDynamicsWorld*				dynamicsWorld;

		btAlignedObjectArray<btCollisionShape*> collisionShapes;

		I4BulletDebugDraw*						debugDrawer;
	};


}
