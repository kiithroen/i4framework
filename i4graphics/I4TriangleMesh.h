#pragma once

#include "I4AABB.h"
using namespace i4core;

namespace i4graphics
{
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

		void 			draw();

		//-------------------------------
		
		I4AABB				localAABB;
		I4VertexBuffer*		vertexBuffer;
		I4IndexBuffer*		indexBuffer;
		bool				skined;
	};
	
}