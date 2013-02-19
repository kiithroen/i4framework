#include "stdafx.h"
#include "I4Mesh.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	I4Mesh::I4Mesh()
		: vertexBuffer(nullptr)
		, indexBuffer(nullptr)
	{
		
	}

	I4Mesh::~I4Mesh()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void I4Mesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void I4Mesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void I4Mesh::draw()
	{
		indexBuffer->draw(I4PT_TRIANGLELIST);
	}
}