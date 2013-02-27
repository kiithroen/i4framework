#pragma once

#include "I4AABB.h"

namespace i4core
{		
	class I4Sphere
	{
	public:
		I4Sphere()
		{
		}

		I4Sphere(const I4Vector3& _center, float _radius)
			: center(_center)
			, radius(_radius)
		{
		}

		const I4Sphere& operator = (const I4Sphere& rhs)
		{
			center = rhs.center;
			radius = rhs.radius;

			return *this;
		}

		void fromAABB(const I4AABB& aabb)
		{
			// 반지름벡터
			center = aabb.maxEdge - aabb.minEdge;
			center *= 0.5f;

			// 반지름길이
			radius = center.getLength();

			// 중점
			center += aabb.minEdge;
		}

		const I4Sphere transform(const I4Matrix4x4& mat) const
		{
			return I4Sphere(mat.transformCoord(center), radius);
		}

		const I4Sphere transformInto(const I4Matrix4x4& mat) const
		{
			I4Matrix4x4 matInv;
			mat.extractInversePrimitive(matInv);

			return transform(matInv);
		}
				
	public:
		I4Vector3	center;
		float		radius;
	};

}