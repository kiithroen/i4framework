#pragma once

#include "I4Matrix4x4.h"

namespace i4core
{
	class Ray
	{
	public:
		Ray()
		{
		}

		Ray(const Vector3& _origin, const Vector3& _direction)
			: origin(_origin)
			, direction(_direction) 
		{
		}

		Ray(const Ray& r)
			: origin(r.origin)
			, direction(r.direction)
		{
		}

		void operator = (const Ray& rhs)
		{
			origin = rhs.origin;
			direction = rhs.direction;
		}

		const Ray transform(const Matrix4x4& mat) const
		{
			return Ray(mat.transformCoord(origin), mat.transformVector(direction));
		}

		bool intersectTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, bool isCull, float* dist)
		{
			Vector3 edge1 = p1 - p0;
			Vector3 edge2 = p2 - p0;

			Vector3 pvec = Vector3::crossProduct(direction, edge2);

			float det = Vector3::dotProduct(edge1, pvec);

			if (isCull && det < 0.0001f)	
				return false;
			
			if (det < 0.0001f && det > -0.0001f)	
				return false;

			Vector3 tvec = origin - p0;
			float u = Vector3::dotProduct(tvec, pvec);
			if (u < 0.0f || u > det)
				return false;

			Vector3 qvec = Vector3::crossProduct(tvec, edge1);
			float v = Vector3::dotProduct(direction, qvec);
			if (v < 0.0f || u + v > det)
				return false;

			if (dist != nullptr)
			{				
				*dist = Vector3::dotProduct(edge2, qvec);
				float invDet = 1.0f / det;
				*dist *= invDet;
			}

			return true;
		}

		bool intersectPlane(const Vector3& N, float D, bool isCull, float* dist)
		{
			float vd = Vector3::dotProduct(N, direction);

			if(fabs(vd) < 0.00001f)
				return false;

			if(isCull && vd > 0.0f)
				return false;

			float vo = -(Vector3::dotProduct(N, origin) + D);
			float _dist = vo / vd;

			if (_dist < 0.0f)
				return false;

			if(dist != 0)
				*dist = _dist;

			return true;
		}

		bool intersectAABB(const Vector3& minEdge, const Vector3& maxEdge, Vector3* hit)
		{
			bool isInside = true;
			Vector3 maxT(-1.0f, -1.0f, -1.0f);
			Vector3 _hit;

			if (origin.x < minEdge.x)
			{
				_hit.x = minEdge.x;
				isInside = false;
				if (direction.x != 0.0f)
				{
					maxT.x = (minEdge.x - origin.x)/direction.x;
				}
			}
			else if (origin.x > maxEdge.x)
			{
				_hit.x = maxEdge.x;
				isInside = false;
				if (direction.x != 0.0f)
				{
					maxT.x = (maxEdge.x - origin.x)/direction.x;
				}
			}

			if (origin.y < minEdge.y)
			{
				_hit.y = minEdge.y;
				isInside = false;
				if (direction.y != 0.0f)
				{
					maxT.y = (minEdge.y - origin.y)/direction.y;
				}
			}
			else if (origin.y > maxEdge.y)
			{
				_hit.y = maxEdge.y;
				isInside = false;
				if (direction.y != 0.0f)
				{
					maxT.y = (maxEdge.y - origin.y)/direction.y;
				}
			}

			if (origin.z < minEdge.z)
			{
				_hit.z = minEdge.z;
				isInside = false;
				if (direction.z != 0.0f)
				{
					maxT.z = (minEdge.z - origin.z)/direction.z;
				}
			}
			else if (origin.z > maxEdge.z)
			{
				_hit.z = maxEdge.z;
				isInside = false;
				if (direction.z != 0.0f)
				{
					maxT.z = (maxEdge.z - origin.z)/direction.z;
				}
			}

			if (isInside)
			{
				if (hit)
				{
					*hit = origin;
					return true;
				}
			}

			int plane = 0;

			if (maxT.y > maxT.xyz[plane]) plane = 1;
			if (maxT.z > maxT.xyz[plane]) plane = 2;

			if (maxT.xyz[plane] < 0)
				return false;

			if (plane != 0)
			{
				_hit.x = origin.x + maxT.x*direction.x;
				if (_hit.x < minEdge.x - 0.00001f || _hit.x > maxEdge.x + 0.00001f)
					return false;
			}

			if (plane != 1)
			{
				_hit.y = origin.y + maxT.y*direction.y;
				if (_hit.y < minEdge.y - 0.00001f || _hit.y > maxEdge.y + 0.00001f)
					return false;
			}

			if (plane != 2)
			{
				_hit.y = origin.y + maxT.y*direction.y;
				if (_hit.y < minEdge.y - 0.00001f || _hit.y > maxEdge.y + 0.00001f)
					return false;
			}
		}

		bool intersectSphere(const Vector3& center, float radius, float* dist)
		{
			Vector3 rsVec = center - origin;
			float rsLength = Vector3::dotProduct(rsVec, rsVec);

			float testA = Vector3::dotProduct(rsVec, direction);

			if (testA < 0 )
				return false;

			float testB = (radius * radius) - rsLength + (testA * testA);

			if (testB < 0)
				return false;

			if (dist != nullptr)
			{
				*dist = testA - sqrt(testB);
			}

			return true;
		}
	public:
		Vector3	origin;
		Vector3	direction;
	};

}