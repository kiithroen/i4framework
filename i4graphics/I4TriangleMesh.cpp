#include "stdafx.h"
#include "I4TriangleMesh.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	I4TriangleMesh::I4TriangleMesh()
		: vertexBuffer(nullptr)
		, indexBuffer(nullptr)
	{
		
	}

	I4TriangleMesh::~I4TriangleMesh()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void I4TriangleMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void I4TriangleMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void I4TriangleMesh::draw()
	{
		indexBuffer->draw(I4PT_TRIANGLELIST);
	}
}