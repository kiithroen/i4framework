#pragma once

#include "I4GeometryBuffer.h"

namespace i4graphics
{

	class I4QuadMesh
	{
	public:
		I4QuadMesh(void);
		~I4QuadMesh(void);

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
