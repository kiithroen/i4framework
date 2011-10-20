#pragma once

#include "i4core.h"
#include "I4Matrix4x4.h"

namespace i4core
{
	class I4Ray
	{
	public:
		I4Ray()
		{
		}

		I4Ray(const I4Vector3& _origin, const I4Vector3& _direction)
			: origin(_origin)
			, direction(_direction) 
		{
		}

		I4Ray(const I4Ray& r)
			: origin(r.origin)
			, direction(r.direction)
		{
		}

		void operator = (const I4Ray& rhs)
		{
			origin = rhs.origin;
			direction = rhs.direction;
		}

		const I4Ray transform(const I4Matrix4x4& mat) const
		{
			return I4Ray(mat.transformCoord(origin), mat.transformVector(direction));
		}

		const I4Ray transformInto(const I4Matrix4x4& mat) const
		{
			I4Matrix4x4 matInv;
			mat.extractInversePrimitive(matInv);

			return transform(matInv);
		}

		bool intersectTriangle(const I4Vector3& p0, const I4Vector3& p1, const I4Vector3& p2, bool isCull, float* dist)
		{
			I4Vector3 edge1 = p1 - p0;
			I4Vector3 edge2 = p2 - p0;

			I4Vector3 pvec = I4Vector3::crossProduct(direction, edge2);

			float det = I4Vector3::dotProduct(edge1, pvec);

			if (isCull && det < 0.0001f)	
				return false;
			
			if (det < 0.0001f && det > -0.0001f)	
				return false;

			I4Vector3 tvec = origin - p0;
			float u = I4Vector3::dotProduct(tvec, pvec);
			if (u < 0.0f || u > det)
				return false;

			I4Vector3 qvec = I4Vector3::crossProduct(tvec, edge1);
			float v = I4Vector3::dotProduct(direction, qvec);
			if (v < 0.0f || u + v > det)
				return false;

			if (dist != NULL)
			{				
				*dist = I4Vector3::dotProduct(edge2, qvec);
				float invDet = 1.0f / det;
				*dist *= invDet;
			}

			return true;
		}

		bool intersectPlane(const I4Vector3& N, float D, bool isCull, float* dist)
		{
			float vd = I4Vector3::dotProduct(N, direction);

			if(fabs(vd) < 0.00001f)
				return false;

			if(isCull && vd > 0.0f)
				return false;

			float vo = -(I4Vector3::dotProduct(N, origin) + D);
			float _dist = vo / vd;

			if (_dist < 0.0f)
				return false;

			if(dist != 0)
				*dist = _dist;

			return true;
		}

		bool intersectAABB(const I4Vector3& minEdge, const I4Vector3& maxEdge, I4Vector3* hit)
		{
			bool isInside = true;
			I4Vector3 maxT(-1.0f, -1.0f, -1.0f);
			I4Vector3 _hit;

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

		bool intersectSphere(const I4Vector3& center, float radius, float* dist)
		{
			I4Vector3 rsVec = center - origin;
			float rsLength = I4Vector3::dotProduct(rsVec, rsVec);

			float testA = I4Vector3::dotProduct(rsVec, direction);

			if (testA < 0 )
				return false;

			float testB = (radius * radius) - rsLength + (testA * testA);

			if (testB < 0)
				return false;

			if (dist != NULL)
			{
				*dist = testA - sqrt(testB);
			}

			return true;
		}
	public:
		I4Vector3	origin;
		I4Vector3	direction;
	};

}