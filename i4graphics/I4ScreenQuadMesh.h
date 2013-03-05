#pragma once

#include "I4GeometryBuffer.h"

namespace i4graphics
{

	class I4ScreenQuadMesh
	{
	public:
		I4ScreenQuadMesh(void);
		~I4ScreenQuadMesh(void);

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
