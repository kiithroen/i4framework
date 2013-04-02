#pragma once

#include "I4Matrix4x4.h"

namespace i4core
{
	enum PlaneLoc { PL_FRONT = 0, PL_BACK = 1, PL_CLIPED = 2 };

	class Plane
	{
	public:
		Plane()
		{
		}

		Plane(const Plane& p)
			: N(p.N), D(p.D)
		{
		}

		Plane(float a, float b, float c, float d)
			: N(a, b, c), D(d)
		{
			normalize();
		}

		Plane(const Vector3& n, float d)
			: N(n), D(d)
		{
		}

		Plane(const Vector3& n, const Vector3& p)
		{
			N = n;
			N.normalize();
			D = -Vector3::dotProduct(N, p);
		}

		Plane(const Vector3& a, const Vector3& b, const Vector3& c)
		{
			Vector3 p0 = b - a;
			Vector3 p1 = c - a;
			N = Vector3::crossProduct(p0, p1);
			N.normalize();

			D = Vector3::dotProduct(N, a);
		}

		const Plane& operator = (const Plane& rhs)
		{
			N = rhs.N;
			D = rhs.D;

			return *this;
		}

		bool operator == (const Plane& rhs)
		{
			return ((D = rhs.D) && (N == rhs.N));
		}

		bool operator != (const Plane& rhs)
		{
			return !(*this == rhs);
		}

		void normalize()
		{		
			float length = N.getLength();

			N /= length;
			D /= length;
		}

		float distanceFrom(const Vector3& p) const
		{
			return (Vector3::dotProduct(N, p) + D);
		}

		const Plane transform(const Matrix4x4& mat) const
		{
			return Plane(mat.transformVector(N), D);
		}

		PlaneLoc testPoint(const Vector3& p) const
		{
			float dp = distanceFrom(p);

			if (dp > 0.0f)
			{
				return PL_FRONT;
			}
			else if (dp < 0.0f)
			{
				return PL_BACK;
			}
			else
			{
				return PL_CLIPED;
			}
		}

		PlaneLoc testTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2)
		{
			PlaneLoc n = testPoint(p0);
			if ((n == testPoint(p1)) && (n == testPoint(p2)))
				return n;

			return PL_CLIPED;
		}

		PlaneLoc testSphere(const Vector3& p, float r) const
		{
			float dp = distanceFrom(p);

			if (dp - r > 0.0f)
			{
				return PL_FRONT;
			}
			else if (dp + r < 0.0f)	
			{
				return PL_BACK;
			}
			else
			{
				return PL_CLIPED;
			}
		}

		PlaneLoc testAABB(const Vector3& minEdge, const Vector3& maxEdge) const
		{
			Vector3 nearEdge(maxEdge);
			Vector3 farEdge(minEdge);

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
				return PL_FRONT;
			}

			else if (distanceFrom(farEdge) > 0.0f)
			{
				return PL_CLIPED;
			}
			else
			{
				return PL_BACK;
			}
		}

	public:
		Vector3	N;
		float		D;
	};
}