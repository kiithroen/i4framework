#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"
#include "I4Hash.h"
#include "I4AABB.h"
using namespace i4core;

namespace i4graphics
{
	class I4VertexBuffer;
	class I4IndexBuffer;
	class I4Texture;

	class I4GRAPHICS_API I4Mesh : public I4HashData<I4Mesh>
	{
	public:
		I4Mesh();
		~I4Mesh();
		
		void			bind();
		void			unbind();

		void 			draw();

		//-------------------------------
		
		I4AABB				localAABB;
		I4VertexBuffer*		vertexBuffer;
		I4IndexBuffer*		indexBuffer;
		I4Texture*			diffuseMap;
		I4Texture*			specularMap;
		I4Texture*			normalMap;
		bool				skined;
	};
	
}