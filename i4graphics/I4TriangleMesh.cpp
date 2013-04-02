#include "stdafx.h"
#include "I4TriangleMesh.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	TriangleMesh::TriangleMesh()
		: vertexBuffer(nullptr)
		, indexBuffer(nullptr)
	{
		
	}

	TriangleMesh::~TriangleMesh()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void TriangleMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void TriangleMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void TriangleMesh::drawAll()
	{
		indexBuffer->draw(PT_TRIANGLELIST);
	}

	void TriangleMesh::drawSub(unsigned int i)
	{
		indexBuffer->draw(PT_TRIANGLELIST, subMeshes[i].count*3, subMeshes[i].start*3, 0);
	}
}