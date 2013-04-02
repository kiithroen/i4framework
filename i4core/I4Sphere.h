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
			// ����������
			center = aabb.maxEdge - aabb.minEdge;
			center *= 0.5f;

			// ����������
			radius = center.getLength();

			// ����
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