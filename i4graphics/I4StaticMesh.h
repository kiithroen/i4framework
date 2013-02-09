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

	class I4GRAPHICS_API I4StaticMesh : public I4HashData<I4StaticMesh>
	{
	public:
		I4StaticMesh();
		~I4StaticMesh();
		
		void			bind();
		void			unbind();

		void 			draw();

		void setVertexBuffer(I4VertexBuffer* vb)
		{
			vertexBuffer = vb;
		}

		void			setIndexBuffer(I4IndexBuffer* ib)	{
			indexBuffer = ib;
		}
		
		
		bool			isSkined() const				{ return skined; }
		void			setSkined(bool _skined)			{ skined = _skined;	}

		I4VertexBuffer*	vertexBuffer;
		I4IndexBuffer*	indexBuffer;
		I4AABB			localAABB;
		bool			skined;
	};
	
}