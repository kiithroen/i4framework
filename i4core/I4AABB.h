#pragma once

#include "i4core.h"
#include "I4Matrix4x4.h"

namespace i4core
{
	class I4AABB
	{
	public:
		I4AABB()
		{
		}

		I4AABB(const I4Vector3& pos)
			: minEdge(pos)
			, maxEdge(pos)
		{
		}

		I4AABB(const I4Vector3& inMin, const I4Vector3& inMax)
			: minEdge(inMin)
			, maxEdge(inMax)
		{
		}

		const I4AABB& operator = (const I4AABB& rhs)
		{
			minEdge = rhs.minEdge;
			maxEdge = rhs.maxEdge;

			return *this;
		}

		void init(const I4Vector3& pos)
		{
			minEdge = pos;
			maxEdge = pos;
		}

		void merge(const I4Vector3& v)
		{
			if (minEdge.x > v.x) minEdge.x = v.x;
			if (minEdge.y > v.y) minEdge.y = v.y;
			if (minEdge.z > v.z) minEdge.z = v.z;
			
			if (maxEdge.x < v.x) maxEdge.x = v.x;
			if (maxEdge.y < v.y) maxEdge.y = v.y;
			if (maxEdge.z < v.z) maxEdge.z = v.z;
		}

		void merge(const I4AABB& box)
		{
			if (minEdge.x > box.minEdge.x) minEdge.x = box.minEdge.x;
			if (minEdge.y > box.minEdge.y) minEdge.y = box.minEdge.y;
			if (minEdge.z > box.minEdge.z) minEdge.z = box.minEdge.z;

			if (maxEdge.x < box.maxEdge.x) maxEdge.x = box.maxEdge.x;
			if (maxEdge.y < box.maxEdge.y) maxEdge.y = box.maxEdge.y;
			if (maxEdge.z < box.maxEdge.z) maxEdge.z = box.maxEdge.z;
		}
		
		
		void repair()
		{
			// 변환이 이루어지고 나서는 최대 최소가 바낄수 있으므로 확인해서 고쳐준다.
			if (minEdge.x > maxEdge.x) swap(minEdge.x, maxEdge.x);
			if (minEdge.y > maxEdge.y) swap(minEdge.y, maxEdge.y);
			if (minEdge.z > maxEdge.z) swap(minEdge.z, maxEdge.z);
		}

		const I4AABB transform(const I4Matrix4x4& mat) const
		{
			I4AABB aabb(mat.transformCoord(minEdge), mat.transformCoord(maxEdge));
			aabb.repair();

			return aabb;
		}

		const I4AABB transformInto(const I4Matrix4x4& mat) const
		{
			I4Matrix4x4 matInv;
			mat.extractInversePrimitive(matInv);

			return transform(matInv);
		}

		// 		  /5--------/6
		// 		 /  |      / |
		// 		/   |     /  |
		// 		1---------2  |
		// 		|   4- - -| -7
		// 		|  /      |  /
		// 		|/        | /
		// 		0---------3/ 

		void I4AABB::extractEdges(I4Vector3* edges) const
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

		void I4AABB::extractEdge(I4Vector3& edge, unsigned int i) const
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

	public:
		I4Vector3	minEdge;
		I4Vector3	maxEdge;		
	};

}