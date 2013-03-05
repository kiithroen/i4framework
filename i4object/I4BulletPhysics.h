#pragma once

#include "btBulletDynamicsCommon.h"

namespace i4object
{
	class I4BulletPhysics
	{
	public:
		I4BulletPhysics(void);
		~I4BulletPhysics(void);

		bool init();
		void destroy();

		void simulate(float dt);

		private:
			btDefaultCollisionConfiguration*		collisionConfiguration;
			btCollisionDispatcher*					dispatcher;
			btBroadphaseInterface*					overlappingPairCache;
			btSequentialImpulseConstraintSolver*	solver;
			btDiscreteDynamicsWorld*				dynamicsWorld;

			btAlignedObjectArray<btCollisionShape*> collisionShapes;
	};
}
