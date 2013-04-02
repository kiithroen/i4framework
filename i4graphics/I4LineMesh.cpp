#include "stdafx.h"
#include "I4LineMesh.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	LineMesh::LineMesh()
		: vertexBuffer(nullptr)
	{
		
	}

	LineMesh::~LineMesh()
	{
		delete vertexBuffer;
	}

	void LineMesh::bind()
	{
		vertexBuffer->bind();
	}

	void LineMesh::unbind()
	{
		vertexBuffer->unbind();
	}

	void LineMesh::draw()
	{
		vertexBuffer->draw(PT_LINELIST);
	}

	void LineMesh::draw(unsigned int count, unsigned int start)
	{
		vertexBuffer->draw(PT_LINELIST, count, start);
	}
}