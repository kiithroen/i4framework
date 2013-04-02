#pragma once

#include "I4Matrix4x4.h"

namespace i4core
{
	class AABB
	{
	public:
		AABB()
			: minEdge(FLT_MAX)
			, maxEdge(-FLT_MAX)
		{
		}

		AABB(const Vector3& pos)
			: minEdge(pos)
			, maxEdge(pos)
		{
		}

		AABB(const Vector3& inMin, const Vector3& inMax)
			: minEdge(inMin)
			, maxEdge(inMax)
		{
		}

		const AABB& operator = (const AABB& rhs)
		{
			minEdge = rhs.minEdge;
			maxEdge = rhs.maxEdge;

			return *this;
		}

		void init(const Vector3& pos)
		{
			minEdge = pos;
			maxEdge = pos;
		}

		void merge(const Vector3& v)
		{
			if (minEdge.x > v.x)
			{
				minEdge.x = v.x;
			}

			if (minEdge.y > v.y)
			{
				minEdge.y = v.y;
			}

			if (minEdge.z > v.z) 
			{
				minEdge.z = v.z;
			}
			
			if (maxEdge.x < v.x)
			{
				maxEdge.x = v.x;
			}

			if (maxEdge.y < v.y)
			{
				maxEdge.y = v.y;
			}

			if (maxEdge.z < v.z)
			{
				maxEdge.z = v.z;
			}
		}

		void merge(const AABB& box)
		{
			if (minEdge.x > box.minEdge.x) minEdge.x = box.minEdge.x;
			if (minEdge.y > box.minEdge.y) minEdge.y = box.minEdge.y;
			if (minEdge.z > box.minEdge.z) minEdge.z = box.minEdge.z;

			if (maxEdge.x < box.maxEdge.x) maxEdge.x = box.maxEdge.x;
			if (maxEdge.y < box.maxEdge.y) maxEdge.y = box.maxEdge.y;
			if (maxEdge.z < box.maxEdge.z) maxEdge.z = box.maxEdge.z;
		}
		
		const AABB transform(const Matrix4x4& mat) const
		{
			Vector3 edges[8];
			extractEdges(edges);

			AABB aabb;
			for (int i = 0; i < 8; ++i)
			{
				edges[i] = mat.transformCoord(edges[i]);
			}

			for (int i = 0; i < 8; ++i)
			{
				aabb.merge(edges[i]);
			}
			return aabb;
		}

		// 		  /5--------/6
		// 		 /  |      / |
		// 		/   |     /  |
		// 		1---------2  |
		// 		|   4- - -| -7
		// 		|  /      |  /
		// 		|/        | /
		// 		0---------3/ 

		void extractEdges(Vector3* edges) const
		{
			edges[0].set(minEdge.x, minEdge.y, minEdge.z);
			edges[1].set(minEdge.x, maxEdge.y, minEdge.z);
			edges[2].set(maxEdge.x, maxEdge.y, minEdge.z);
			edges[3].set(maxEdge.x, minEdge.y, minEdge.z);
			edges[4].set(minEdge.x, minEdge.y, maxEdge.z);
			edges[5].set(minEdge.x, maxEdge.y, maxEdge.z);
			edges[6].set(maxEdge.x, maxEdge.y, maxEdge.z);
			edges[7].set(maxEdge.x, minEdge.y, maxEdge.z);
		}

		void extractEdge(Vector3& edge, unsigned int i) const
		{
			switch (i)
			{
			case 0:
				edge.set(minEdge.x, minEdge.y, minEdge.z);
				break;
			case 1:
				edge.set(minEdge.x, maxEdge.y, minEdge.z);
				break;
			case 2:
				edge.set(maxEdge.x, maxEdge.y, minEdge.z);
				break;
			case 3:
				edge.set(maxEdge.x, minEdge.y, minEdge.z);
				break;
			case 4:
				edge.set(minEdge.x, minEdge.y, maxEdge.z);
				break;
			case 5:
				edge.set(minEdge.x, maxEdge.y, maxEdge.z);
				break;
			case 6:
				edge.set(maxEdge.x, maxEdge.y, maxEdge.z);
				break;
			case 7:
				edge.set(maxEdge.x, minEdge.y, maxEdge.z);
				break;
			default:
				assert(false);
				break;
			}
		}

		Vector3 getCenter()
		{
			return (minEdge + maxEdge)*VECTOR3_HALF;
		}

		Vector3 getExtent()
		{
			return (maxEdge - minEdge)*VECTOR3_HALF;
		}

	public:
		Vector3	minEdge;
		Vector3	maxEdge;		
	};

}