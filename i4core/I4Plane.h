#pragma once

#include "i4core.h"
#include "I4Matrix4x4.h"

namespace i4core
{
	enum I4PlaneLoc { I4PL_FRONT = 0, I4PL_BACK = 1, I4PL_CLIPED = 2 };

	class I4Plane
	{
	public:
		I4Plane()
		{
		}

		I4Plane(const I4Plane& p)
			: N(p.N), D(p.D)
		{
		}

		I4Plane(float a, float b, float c, float d)
			: N(a, b, c), D(d)
		{
			normalize();
		}

		I4Plane(const I4Vector3& n, float d)
			: N(n), D(d)
		{
		}

		I4Plane(const I4Vector3& n, const I4Vector3& p)
		{
			N = n;
			N.normalize();
			D = -I4Vector3::dotProduct(N, p);
		}

		I4Plane(const I4Vector3& a, const I4Vector3& b, const I4Vector3& c)
		{
			I4Vector3 p0 = b - a;
			I4Vector3 p1 = c - a;
			N = I4Vector3::crossProduct(p0, p1);
			N.normalize();

			D = I4Vector3::dotProduct(N, a);
		}

		const I4Plane& operator = (const I4Plane& rhs)
		{
			N = rhs.N;
			D = rhs.D;

			return *this;
		}

		bool operator == (const I4Plane& rhs)
		{
			return ((D = rhs.D) && (N == rhs.N));
		}

		bool operator != (const I4Plane& rhs)
		{
			return !(*this == rhs);
		}

		void normalize()
		{		
			float length = N.getLength();

			N /= length;
			D /= length;
		}

		float distanceFrom(const I4Vector3& p) const
		{
			return (I4Vector3::dotProduct(N, p) + D);
		}

		const I4Plane transform(const I4Matrix4x4& mat) const
		{
			return I4Plane(mat.transformVector(N), D);
		}

		const I4Plane transformInto(const I4Matrix4x4& mat) const
		{
			I4Matrix4x4 matInv;
			mat.extractInversePrimitive(matInv);

			return transform(matInv);
		}

		I4PlaneLoc testPoint(const I4Vector3& p) const
		{
			float dp = distanceFrom(p);

			if (dp > 0.0f)
			{
				return I4PL_FRONT;
			}
			else if (dp < 0.0f)
			{
				return I4PL_BACK;
			}
			else
			{
				return I4PL_CLIPED;
			}
		}

		I4PlaneLoc testTriangle(const I4Vector3& p0, const I4Vector3& p1, const I4Vector3& p2)
		{
			I4PlaneLoc n = testPoint(p0);
			if ((n == testPoint(p1)) && (n == testPoint(p2)))
				return n;

			return I4PL_CLIPED;
		}

		I4PlaneLoc testSphere(const I4Vector3& p, float r) const
		{
			float dp = distanceFrom(p);

			if (dp - r > 0.0f)
			{
				return I4PL_FRONT;
			}
			else if (dp + r < 0.0f)	
			{
				return I4PL_BACK;
			}
			else
			{
				return I4PL_CLIPED;
			}
		}

		I4PlaneLoc testAABB(const I4Vector3& minEdge, const I4Vector3& maxEdge) const
		{
			I4Vector3 nearEdge(maxEdge);
			I4Vector3 farEdge(minEdge);

			if (N.x > 0.0f)
			{
				nearEdge.x = minEdge.x;
				farEdge.x = maxEdge.x;
			}

			if (N.y > 0.0f)
			{
				nearEdge.y = minEdge.y;
				farEdge.y = maxEdge.y;
			}

			if (N.z > 0.0f)
			{
				nearEdge.z = minEdge.z;
				farEdge.z = maxEdge.z;
			}

			if (distanceFrom(nearEdge) > 0.0f)
			{
				return I4PL_FRONT;
			}

			else if (distanceFrom(farEdge) > 0.0f)
			{
				return I4PL_CLIPED;
			}
			else
			{
				return I4PL_BACK;
			}
		}

	public:
		I4Vector3	N;
		float		D;
	};
}