#include "stdafx.h"
#include "I4BulletPhysics.h"
#include "I4Log.h"
using namespace i4core;

namespace i4object
{
	I4BulletPhysics::I4BulletPhysics(void)
	{
	}

	I4BulletPhysics::~I4BulletPhysics()
	{
		destroy();
	}

	bool I4BulletPhysics::init()
	{
		return true;
	}

	void I4BulletPhysics::destroy()
	{
	}	
}
