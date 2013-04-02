#pragma once

namespace i4graphics
{
	class VertexBuffer;
	class IndexBuffer;
	class Texture;

	class LineMesh
	{
	public:
		LineMesh();
		~LineMesh();
		
		void			bind();
		void			unbind();

		void 			draw();
		void			draw(unsigned int count, unsigned int start);

		//-------------------------------
		
		VertexBuffer*		vertexBuffer;
	};
	
}