#pragma once

namespace i4graphics
{
	class I4VertexBuffer;
	class I4IndexBuffer;
	class I4Texture;

	class I4LineMesh
	{
	public:
		I4LineMesh();
		~I4LineMesh();
		
		void			bind();
		void			unbind();

		void 			draw();
		void			draw(unsigned int count, unsigned int start);

		//-------------------------------
		
		I4VertexBuffer*		vertexBuffer;
	};
	
}