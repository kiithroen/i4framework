#include "I4QuadMesh.h"
#include "I4VideoDriver.h"

namespace i4graphics
{
	I4QuadMesh::I4QuadMesh(void)
	{
	}


	I4QuadMesh::~I4QuadMesh(void)
	{
		destroy();
	}

	void I4QuadMesh::create()
	{
		const static I4Vertex_Pos_Tex quad[] =
		{			
			{ I4Vector3(-1.0f, -1.0f, 0.0f), I4TextureUV(0.0f, 1.0f) },
			{ I4Vector3(-1.0f, 1.0f, 0.0f), I4TextureUV(0.0f, 0.0f) },
			{ I4Vector3(1.0f, 1.0f, 0.0f), I4TextureUV(1.0f, 0.0f) },
			{ I4Vector3(1.0f, -1.0f, 0.0f), I4TextureUV(1.0f, 1.0f) },
		};

		vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
		vertexBuffer->create(_countof(quad), sizeof(I4Vertex_Pos_Tex), quad);

		const static unsigned short indices[] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		indexBuffer = I4VideoDriver::getVideoDriver()->createIndexBuffer();
		indexBuffer->create(_countof(indices), sizeof(unsigned short), indices);
	}

	void I4QuadMesh::destroy()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void I4QuadMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void I4QuadMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void I4QuadMesh::draw()
	{		
		indexBuffer->draw(I4PT_TRIANGLELIST);
	}
}
