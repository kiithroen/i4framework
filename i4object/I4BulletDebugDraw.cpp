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


	void I4BulletDebugDraw::drawLine(const I4Vector3& from,const I4Vector3& to,const I4Vector3& color)
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

	void I4BulletDebugDraw::draw3dText(const I4Vector3& location,const char* textString)
	{
	}

	void I4BulletDebugDraw::reportErrorWarning(const char* warningString)
	{
		I4LOG_WARN << warningString;
	}

	void I4BulletDebugDraw::drawContactPoint(const I4Vector3& pointOnB,const I4Vector3& normalOnB,float distance,int lifeTime,const I4Vector3& color)
	{
		I4Vector3 to=pointOnB+normalOnB*1;//distance;
		const I4Vector3&from = pointOnB;
		drawLine(from, to, color);
	}

}