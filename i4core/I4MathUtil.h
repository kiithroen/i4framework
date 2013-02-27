#pragma once

namespace i4core
{
	static const float	I4EPSILON		= 0.001f;
	static const float	I4PI			= 3.141592654f;
	static const float	I4DEGTORAD		= I4PI/180.0f;
	static const float	I4RADTODEG		= 180.0f/I4PI;

	class I4MathUtil
	{
	public:
		static bool isEpsilonZero(float v)
		{
			return (v > -I4EPSILON && v < I4EPSILON) ? true : false;
		}

		static bool isEpsilonEqual(float a, float b)
		{
			return isEpsilonZero(a - b);
		}

		static float degreeToRadian(float deg)
		{
			return I4DEGTORAD*deg;
		}

		static float radianToDegree(float rad)
		{
			return I4RADTODEG*rad;
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