#include "stdafx.h"
#include "I4LineMesh.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	I4LineMesh::I4LineMesh()
		: vertexBuffer(nullptr)
	{
		
	}

	I4LineMesh::~I4LineMesh()
	{
		delete vertexBuffer;
	}

	void I4LineMesh::bind()
	{
		vertexBuffer->bind();
	}

	void I4LineMesh::unbind()
	{
		vertexBuffer->unbind();
	}

	void I4LineMesh::draw()
	{
		vertexBuffer->draw(I4PT_LINELIST);
	}

	void I4LineMesh::draw(unsigned int count, unsigned int start)
	{
		vertexBuffer->draw(I4PT_LINELIST, count, start);
	}
}