#pragma once

#include "i4core.h"

namespace i4core
{
	static const float	EPSILON		= 0.001f;
	static const float	PI			= 3.141592654f;
	static const float	DEGTORAD	= PI / 180.0f;
	static const float	RADTODEG	= 180.0f / PI;

	class I4MathUtil
	{
	public:
		static bool isEpsilonZero(float v)
		{
			return (v > -EPSILON && v < EPSILON) ? true : false;
		}

		static bool isEpsilonEqual(float a, float b)
		{
			return isEpsilonZero(a - b);
		}

		static float degreeToRadian(float deg)
		{
			return DEGTORAD*deg;
		}

		static float radianToDegree(float rad)
		{
			return RADTODEG*rad;
		}

		template <typename T>
		static T lerp(const T& a, const T& b, float t)
		{
			return (a - (a*t) + (b*t));
		}

		static float percent(float a, float b, float val)	// val이 a, b사이에서 몇 퍼센트인가?
		{
			return (val - a)/(b - a);
		}

	};
}