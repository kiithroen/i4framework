#pragma once

#include "I4GeometryBuffer.h"

namespace i4graphics
{

	class ScreenQuadMesh
	{
	public:
		ScreenQuadMesh(void);
		~ScreenQuadMesh(void);

		bool	create();
		void	destroy();

		void	bind();
		void	unbind();

		void	draw();

	private:
		VertexBuffer*	vertexBuffer;
		IndexBuffer*	indexBuffer;
	};

}
