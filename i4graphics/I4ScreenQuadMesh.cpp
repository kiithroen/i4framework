#include "stdafx.h"
#include "I4ScreenQuadMesh.h"
#include "I4VideoDriver.h"

namespace i4graphics
{
	I4ScreenQuadMesh::I4ScreenQuadMesh(void)
	{
	}


	I4ScreenQuadMesh::~I4ScreenQuadMesh(void)
	{
		destroy();
	}

	bool I4ScreenQuadMesh::create()
	{
		const static I4Vertex_Pos_Tex quad[] =
		{			
			{ I4Vector3(-1.0f, -1.0f, 0.0f), I4TextureUV(0.0f, 1.0f) },
			{ I4Vector3(-1.0f, 1.0f, 0.0f), I4TextureUV(0.0f, 0.0f) },
			{ I4Vector3(1.0f, 1.0f, 0.0f), I4TextureUV(1.0f, 0.0f) },
			{ I4Vector3(1.0f, -1.0f, 0.0f), I4TextureUV(1.0f, 1.0f) },
		};

		vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
		if (vertexBuffer->create(_countof(quad), sizeof(I4Vertex_Pos_Tex), quad) == false)
		{
			destroy();
			return false;
		}

		const static unsigned short indices[] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		indexBuffer = I4VideoDriver::getVideoDriver()->createIndexBuffer();
		if (indexBuffer->create(_countof(indices), sizeof(unsigned short), indices) == false)
		{
			destroy();
			return false;
		}

		return true;
	}

	void I4ScreenQuadMesh::destroy()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void I4ScreenQuadMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void I4ScreenQuadMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void I4ScreenQuadMesh::draw()
	{		
		indexBuffer->draw(I4PT_TRIANGLELIST);
	}
}
