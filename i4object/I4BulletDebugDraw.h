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

		virtual void	drawLine(const I4Vector3& from,const I4Vector3& to,const I4Vector3& color) override;
		virtual void	drawContactPoint(const I4Vector3& PointOnB,const I4Vector3& normalOnB,float distance,int lifeTime,const I4Vector3& color) override;
		virtual void	reportErrorWarning(const char* warningString) override;
		virtual void	draw3dText(const I4Vector3& location,const char* textString) override;
		virtual void	setDebugMode(int debugMode) override;

		virtual int		getDebugMode() const { return debugMode;}

	private:
		I4Renderer*		renderer;
		int				debugMode;
	};
}