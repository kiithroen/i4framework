#pragma once

#include "i4core.h"
#include "I4Matrix4x4.h"

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

		const I4Sphere transform(const I4Matrix4x4& mat) const
		{
			return I4Sphere(mat.transformCoord(center), radius);
		}

		const I4Sphere transformInto(const I4Matrix4x4& mat) const
		{
			I4Matrix4x4 matInv;
			mat.getInversePrimitive(matInv);

			return transform(matInv);
		}
				
	public:
		I4Vector3	center;
		float		radius;
	};

}