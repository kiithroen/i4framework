#pragma once

#include "I4AABB.h"

namespace i4core
{		
	class Sphere
	{
	public:
		Sphere()
		{
		}

		Sphere(const Vector3& _center, float _radius)
			: center(_center)
			, radius(_radius)
		{
		}

		const Sphere& operator = (const Sphere& rhs)
		{
			center = rhs.center;
			radius = rhs.radius;

			return *this;
		}

		void fromAABB(const AABB& aabb)
		{
			// 반지름벡터
			center = aabb.maxEdge - aabb.minEdge;
			center *= 0.5f;

			// 반지름길이
			radius = center.getLength();

			// 중점
			center += aabb.minEdge;
		}

		const Sphere transform(const Matrix4x4& mat) const
		{
			return Sphere(mat.transformCoord(center), radius);
		}
				
	public:
		Vector3	center;
		float		radius;
	};

}