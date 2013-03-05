#pragma once

#include "I4GeometryBuffer.h"

namespace i4graphics
{

	class I4PointLightMesh
	{
	public:
		I4PointLightMesh(void);
		~I4PointLightMesh(void);

		bool	create(float r, unsigned short ringCount, unsigned short segmentCount);
		void	destroy();

		void	bind();
		void	unbind();

		void	draw();

	private:
		I4VertexBuffer*	vertexBuffer;
		I4IndexBuffer*	indexBuffer;
	};

}
