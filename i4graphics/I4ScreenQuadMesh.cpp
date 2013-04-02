#include "stdafx.h"
#include "I4ScreenQuadMesh.h"
#include "I4VideoDriver.h"

namespace i4graphics
{
	ScreenQuadMesh::ScreenQuadMesh(void)
	{
	}


	ScreenQuadMesh::~ScreenQuadMesh(void)
	{
		destroy();
	}

	bool ScreenQuadMesh::create()
	{
		const static Vertex_Pos_Tex quad[] =
		{			
			{ Vector3(-1.0f, -1.0f, 0.0f), TextureUV(0.0f, 1.0f) },
			{ Vector3(-1.0f, 1.0f, 0.0f), TextureUV(0.0f, 0.0f) },
			{ Vector3(1.0f, 1.0f, 0.0f), TextureUV(1.0f, 0.0f) },
			{ Vector3(1.0f, -1.0f, 0.0f), TextureUV(1.0f, 1.0f) },
		};

		vertexBuffer = VideoDriver::getVideoDriver()->createVertexBuffer();
		if (vertexBuffer->create(_countof(quad), sizeof(Vertex_Pos_Tex), quad) == false)
		{
			destroy();
			return false;
		}

		const static unsigned short indices[] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		indexBuffer = VideoDriver::getVideoDriver()->createIndexBuffer();
		if (indexBuffer->create(_countof(indices), sizeof(unsigned short), indices) == false)
		{
			destroy();
			return false;
		}

		return true;
	}

	void ScreenQuadMesh::destroy()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void ScreenQuadMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void ScreenQuadMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void ScreenQuadMesh::draw()
	{		
		indexBuffer->draw(PT_TRIANGLELIST);
	}
}
