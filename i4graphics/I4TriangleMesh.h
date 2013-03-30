#pragma once

#include "I4AABB.h"
using namespace i4core;

namespace i4graphics
{
	struct I4SubMesh
	{
		int id;
		int start;
		int count;
	};

	class I4VertexBuffer;
	class I4IndexBuffer;
	class I4Texture;

	class I4TriangleMesh
	{
	public:
		I4TriangleMesh();
		~I4TriangleMesh();
		
		void			bind();
		void			unbind();

		void 			drawAll();
		void			drawSub(unsigned int i);

		//-------------------------------
		
		I4AABB				localAABB;
		I4VertexBuffer*		vertexBuffer;
		I4IndexBuffer*		indexBuffer;
		vector<I4SubMesh>	subMeshes;
		bool				skined;
	};
	
}