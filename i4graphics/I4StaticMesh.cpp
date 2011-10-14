#include "I4StaticMesh.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	I4StaticMesh::I4StaticMesh()
		: vertexBuffer(NULL)
		, indexBuffer(NULL)
	{
		
	}

	I4StaticMesh::~I4StaticMesh()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void I4StaticMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void I4StaticMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void I4StaticMesh::draw()
	{
		if (vertexBuffer != NULL && indexBuffer != NULL)
		{
			indexBuffer->draw(I4PT_TRIANGLELIST);
		}
	}
}