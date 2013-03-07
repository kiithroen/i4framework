#pragma once

#include "btBulletDynamicsCommon.h"

namespace i4graphics
{
	class I4Renderer;
}
using namespace i4graphics;

namespace i4object
{
	class I4BulletDebugDraw : public btIDebugDraw
	{
	public:
		I4BulletDebugDraw(I4Renderer* renderer);
		virtual ~I4BulletDebugDraw(void);

		virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;
		virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override;
		virtual void	reportErrorWarning(const char* warningString) override;
		virtual void	draw3dText(const btVector3& location,const char* textString) override;
		virtual void	setDebugMode(int debugMode) override;

		virtual int		getDebugMode() const { return debugMode;}

	private:
		I4Renderer*		renderer;
		int				debugMode;
	};
}