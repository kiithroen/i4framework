#pragma once

// i4core
#include "i4core.h"

namespace i4core
{
	class I4Log;
	class I4Profile;
	class I4ProfileJson;
	class I4StopWatch;
	class I4Vector2;
	class I4Vector3;
	class I4Vector4;
	class I4Matrix4x4;
	class I4Quaternion;	
	class I4Ray;
	class I4Plane;
	class I4Sphere;
	class I4AABB;
}

using namespace i4core;

// dll macro
#ifdef I4GRAPHICS_EXPORTS
#define I4GRAPHICS_API __declspec(dllexport)
#else
#define I4GRAPHICS_API __declspec(dllimport)
#endif
