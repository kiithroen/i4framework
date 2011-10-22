#pragma once

#include "i4core.h"

namespace i4core
{

namespace mathutil
{
	static const float	EPSILON		= 0.001f;
	static const float	PI			= 3.141592654f;
	static const float	DEGTORAD	= PI / 180.0f;
	static const float	RADTODEG	= 180.0f / PI;

	inline bool isEpsilonZero(float v)
	{
		return (v > -EPSILON && v < EPSILON) ? true : false;
	}

	inline bool isEpsilonEqual(float a, float b)
	{
		return isEpsilonZero(a - b);
	}

	inline float degreeToRadian(float deg)
	{
		return DEGTORAD*deg;
	}

	inline float radianToDegree(float rad)
	{
		return RADTODEG*rad;
	}

	template <typename T>
	inline T lerp(const T& a, const T& b, float t)
	{
		return (a - (a*t) + (b*t));
	}

	inline float percent(float a, float b, float val)	// val이 a, b사이에서 몇 퍼센트인가?
	{
		return (val - a)/(b - a);
	}

}

}