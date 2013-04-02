#pragma once

#include "I4Vector3.h"
#include "I4Matrix4x4.h"
#include "I4Plane.h"
#include "I4AABB.h"
#include "I4Sphere.h"

namespace i4core
{
	enum FrustumPlane
	{
		FP_LEFT = 0,
		FP_RIGHT,
		FP_TOP,
		FP_BOTTOM,
		FP_NEAR,
		FP_FAR
	};

	class Frustum
	{
	public:
		Frustum()
		{
		}

		void make(const Matrix4x4& mat)
		{
			// 참고자료
			// Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix
			// Gil Gribb, Klaus Hartmann

			// mat -> projection : view space clipping plane
			// mat -> view*projection : world space clipping plane
			// mat -> world*view*projection : object space clipping plane

			planes[FP_LEFT].N.x = mat._14 + mat._11; // Left
			planes[FP_LEFT].N.y = mat._24 + mat._21;
			planes[FP_LEFT].N.z = mat._34 + mat._31;
			planes[FP_LEFT].D = mat._44 + mat._41;
			planes[FP_LEFT].normalize();

			planes[FP_RIGHT].N.x = mat._14 - mat._11; // Right
			planes[FP_RIGHT].N.y = mat._24 - mat._21;
			planes[FP_RIGHT].N.z = mat._34 - mat._31;
			planes[FP_RIGHT].D = mat._44 - mat._41;
			planes[FP_RIGHT].normalize();

			planes[FP_TOP].N.x = mat._14 - mat._12; // Top
			planes[FP_TOP].N.y = mat._24 - mat._22;
			planes[FP_TOP].N.z = mat._34 - mat._32;
			planes[FP_TOP].D = mat._44 - mat._42;
			planes[FP_TOP].normalize();

			planes[FP_BOTTOM].N.x = mat._14 + mat._12; // Bottom
			planes[FP_BOTTOM].N.y = mat._24 + mat._22;
			planes[FP_BOTTOM].N.z = mat._34 + mat._32;
			planes[FP_BOTTOM].D = mat._44 + mat._42;
			planes[FP_BOTTOM].normalize();

			planes[FP_NEAR].N.x = mat._13; // Near
			planes[FP_NEAR].N.y = mat._23;
			planes[FP_NEAR].N.z = mat._33;
			planes[FP_NEAR].D = mat._43;
			planes[FP_NEAR].normalize();

			planes[FP_FAR].N.x = mat._14 - mat._13; // Far
			planes[FP_FAR].N.y = mat._24 - mat._23;
			planes[FP_FAR].N.z = mat._34 - mat._33;
			planes[FP_FAR].D = mat._44 - mat._43;
			planes[FP_FAR].normalize();
		}

		bool isInPoint(const Vector3& p) const
		{
			for (int i = 0; i < 6; i++)
			{
				if (planes[i].testPoint(p) == PL_BACK)
					return false;
			}

			return true;
		}

		bool isInSphere(const Vector3& p, float r) const
		{
			for (int i = 0; i < 6; i++)
			{
				if (planes[i].testSphere(p, r) == PL_BACK)
					return false;
			}

			return true;
		}

		bool isInSphere(const Sphere& sphere) const
		{
			return isInSphere(sphere.center, sphere.radius);
		}

		bool isInAABB(const AABB& aabb) const
		{
			Vector3 edge;
			for (int i = 0; i < 6; ++i)
			{
				bool inside = false;
				for (int j = 0; j < 8; ++j)
				{
					aabb.extractEdge(edge, j);
					if (planes[i].testPoint(edge) == PL_FRONT)
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
		Plane			planes[6];
	};

}