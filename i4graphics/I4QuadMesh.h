#pragma once

#include "i4graphics.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{

	class I4GRAPHICS_API I4QuadMesh
	{
	public:
		I4QuadMesh(void);
		~I4QuadMesh(void);

		void	create();
		void	destroy();

		void	bind();
		void	unbind();

		void	draw();

	private:
		I4VertexBuffer*	vertexBuffer;
		I4IndexBuffer*	indexBuffer;
	};

}
