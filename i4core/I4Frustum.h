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

		void make(const I4Matrix4x4& mat)
		{
			// 참고자료
			// Fast Extraction of Viewing I4Frustum Planes from the World-View-Projection Matrix
			// Gil Gribb, Klaus Hartmann

			// mat -> projection : view space clipping plane
			// mat -> view*projection : world space clipping plane
			// mat -> world*view*projection : object space clipping plane

			planes[I4FP_LEFT].N.x = mat._14 + mat._11; // Left
			planes[I4FP_LEFT].N.y = mat._24 + mat._21;
			planes[I4FP_LEFT].N.z = mat._34 + mat._31;
			planes[I4FP_LEFT].D = mat._44 + mat._41;
			planes[I4FP_LEFT].normalize();

			planes[I4FP_RIGHT].N.x = mat._14 - mat._11; // Right
			planes[I4FP_RIGHT].N.y = mat._24 - mat._21;
			planes[I4FP_RIGHT].N.z = mat._34 - mat._31;
			planes[I4FP_RIGHT].D = mat._44 - mat._41;
			planes[I4FP_RIGHT].normalize();

			planes[I4FP_TOP].N.x = mat._14 - mat._12; // Top
			planes[I4FP_TOP].N.y = mat._24 - mat._22;
			planes[I4FP_TOP].N.z = mat._34 - mat._32;
			planes[I4FP_TOP].D = mat._44 - mat._42;
			planes[I4FP_TOP].normalize();

			planes[I4FP_BOTTOM].N.x = mat._14 + mat._12; // Bottom
			planes[I4FP_BOTTOM].N.y = mat._24 + mat._22;
			planes[I4FP_BOTTOM].N.z = mat._34 + mat._32;
			planes[I4FP_BOTTOM].D = mat._44 + mat._42;
			planes[I4FP_BOTTOM].normalize();

			planes[I4FP_NEAR].N.x = mat._13; // Near
			planes[I4FP_NEAR].N.y = mat._23;
			planes[I4FP_NEAR].N.z = mat._33;
			planes[I4FP_NEAR].D = mat._43;
			planes[I4FP_NEAR].normalize();

			planes[I4FP_FAR].N.x = mat._14 - mat._13; // Far
			planes[I4FP_FAR].N.y = mat._24 - mat._23;
			planes[I4FP_FAR].N.z = mat._34 - mat._33;
			planes[I4FP_FAR].D = mat._44 - mat._43;
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

		bool isInSphere(const I4Vector3& p, float r) const
		{
			for (int i = 0; i < 6; i++)
			{
				if (planes[i].testSphere(p, r) == I4PL_BACK)
					return false;
			}

			return true;
		}

		bool isInSphere(const I4Sphere& sphere) const
		{
			return isInSphere(sphere.center, sphere.radius);
		}

		bool isInAABB(const I4AABB& aabb) const
		{
			I4Vector3 edge;
			for (int i = 0; i < 6; ++i)
			{
				bool inside = false;
				for (int j = 0; j < 8; ++j)
				{
					aabb.extractEdge(edge, j);
					if (planes[i].testPoint(edge) == I4PL_FRONT)
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