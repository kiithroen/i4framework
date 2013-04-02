#pragma once

#include "I4GeometryBuffer.h"

namespace i4graphics
{

	class PointLightMesh
	{
	public:
		PointLightMesh(void);
		~PointLightMesh(void);

		bool	create(float r, unsigned short ringCount, unsigned short segmentCount);
		void	destroy();

		void	bind();
		void	unbind();

		void	draw();

	private:
		VertexBuffer*	vertexBuffer;
		IndexBuffer*	indexBuffer;
	};

}
