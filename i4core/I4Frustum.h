#pragma once

#include "i4core.h"
#include "I4Vector3.h"
#include "I4Matrix4x4.h"
#include "I4Plane.h"
#include "I4AABB.h"
#include "I4Sphere.h"

namespace i4core
{
	enum I4FrustumPlane
	{
		I4FP_LEFT = 0,
		I4FP_RIGHT,
		I4FP_TOP,
		I4FP_BOTTOM,
		I4FP_NEAR,
		I4FP_FAR
	};

	class I4Frustum
	{
	public:
		I4Frustum()
		{
		}

		void make(const I4Matrix4x4& viewProj)
		{
			// 참고자료
			// http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf
			// Fast Extraction of Viewing I4Frustum Planes from the World-View-Projection Matrix
			// Gil Gribb, Klaus Hartmann

			planes[I4FP_LEFT].N.x = viewProj._14 + viewProj._11; // Left
			planes[I4FP_LEFT].N.y = viewProj._24 + viewProj._21;
			planes[I4FP_LEFT].N.z = viewProj._34 + viewProj._31;
			planes[I4FP_LEFT].D = viewProj._44 + viewProj._41;
			planes[I4FP_LEFT].normalize();

			planes[I4FP_RIGHT].N.x = viewProj._14 - viewProj._11; // Right
			planes[I4FP_RIGHT].N.y = viewProj._24 - viewProj._21;
			planes[I4FP_RIGHT].N.z = viewProj._34 - viewProj._31;
			planes[I4FP_RIGHT].D = viewProj._44 - viewProj._41;
			planes[I4FP_RIGHT].normalize();

			planes[I4FP_TOP].N.x = viewProj._14 - viewProj._12; // Top
			planes[I4FP_TOP].N.y = viewProj._24 - viewProj._22;
			planes[I4FP_TOP].N.z = viewProj._34 - viewProj._32;
			planes[I4FP_TOP].D = viewProj._44 - viewProj._42;
			planes[I4FP_TOP].normalize();

			planes[I4FP_BOTTOM].N.x = viewProj._14 + viewProj._12; // Bottom
			planes[I4FP_BOTTOM].N.y = viewProj._24 + viewProj._22;
			planes[I4FP_BOTTOM].N.z = viewProj._34 + viewProj._32;
			planes[I4FP_BOTTOM].D = viewProj._44 + viewProj._42;
			planes[I4FP_BOTTOM].normalize();

			planes[I4FP_NEAR].N.x = viewProj._14 + viewProj._13; // Near
			planes[I4FP_NEAR].N.y = viewProj._24 + viewProj._23;
			planes[I4FP_NEAR].N.z = viewProj._34 + viewProj._33;
			planes[I4FP_NEAR].D = viewProj._44 + viewProj._43;
			planes[I4FP_NEAR].normalize();

			planes[I4FP_FAR].N.x = viewProj._14 - viewProj._13; // Far
			planes[I4FP_FAR].N.y = viewProj._24 - viewProj._23;
			planes[I4FP_FAR].N.z = viewProj._34 - viewProj._33;
			planes[I4FP_FAR].D = viewProj._44 - viewProj._43;
			planes[I4FP_FAR].normalize();
		}

		bool isInPoint(const I4Vector3& p) const
		{
			for (int i = 0; i < 6; i++)
			{
				if (planes[i].testPoint(p) == I4PL_BACK)
					return false;
			}

			return true;
		}

		bool isInSphere(const I4Sphere& sphere) const
		{
			for (int i = 0; i < 6; i++)
			{
				if (planes[i].testSphere(sphere.center, sphere.radius) == I4PL_FRONT)
					return false;
			}

			return true;
		}

		bool isInAABB(const I4AABB& aabb) const
		{
			I4Vector3 edges[8];
			aabb.getEdges(edges);

			for (int i = 0; i < 6; ++i)
			{
				bool inside = false;
				for (int j=0; j<8; ++j)
				{
					if (planes[i].testPoint(edges[j]) == I4PL_FRONT)
					{
						inside = true;
						break;
					}
				}

				if (inside == false)
					return false;
			}

			return true;
		}

	public:
		I4Plane			planes[6];
	};

}