#include "stdafx.h"
#include "I4BulletDebugDraw.h"
#include "I4Renderer.h"
#include "I4Log.h"
using namespace i4core;

namespace i4object
{

	I4BulletDebugDraw::I4BulletDebugDraw(I4Renderer* _renderer)
		: renderer(_renderer)
		, debugMode(0)
	{

	}

	I4BulletDebugDraw::~I4BulletDebugDraw()
	{
	}


	void I4BulletDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
	{
		I4DebugLine line;
		line.p0 = I4Vector3(from.x(), from.y(), from.z());
		line.p1 = I4Vector3(to.x(), to.y(), to.z());
		line.color0 = I4Vector4(color.x(), color.y(), color.z(), 1);
		line.color1 = I4Vector4(color.x(), color.y(), color.z(), 1);
		line.life = -1;	// 바로 삭제.
		renderer->commit(line);
	}
	
	void I4BulletDebugDraw::setDebugMode(int _debugMode)
	{
		debugMode = _debugMode;
	}

	void I4BulletDebugDraw::draw3dText(const btVector3& location,const char* textString)
	{
	}

	void I4BulletDebugDraw::reportErrorWarning(const char* warningString)
	{
		I4LOG_WARN << warningString;
	}

	void I4BulletDebugDraw::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
	{
		btVector3 to=pointOnB+normalOnB*1;//distance;
		const btVector3&from = pointOnB;
		drawLine(from, to, color);
	}

}