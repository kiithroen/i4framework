#pragma once

#include "i4graphics.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{

	class I4SphereMesh
	{
	public:
		I4SphereMesh(void);
		~I4SphereMesh(void);

		bool	create();
		void	destroy();

		void	bind();
		void	unbind();

		void	draw();

	private:
		I4VertexBuffer*	vertexBuffer;
		I4IndexBuffer*	indexBuffer;
	};

}
