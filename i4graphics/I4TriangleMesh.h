#pragma once

#include "I4AABB.h"
using namespace i4core;

namespace i4graphics
{
	struct SubMesh
	{
		int id;
		int start;
		int count;
	};

	class VertexBuffer;
	class IndexBuffer;
	class Texture;

	class TriangleMesh
	{
	public:
		TriangleMesh();
		~TriangleMesh();
		
		void			bind();
		void			unbind();

		void 			drawAll();
		void			drawSub(unsigned int i);

		//-------------------------------
		
		AABB				localAABB;
		VertexBuffer*		vertexBuffer;
		IndexBuffer*		indexBuffer;
		vector<SubMesh>	subMeshes;
		bool				skined;
	};
	
}