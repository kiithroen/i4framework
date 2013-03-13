#include "stdafx.h"
#include "I4LineBatch.h"
#include "I4LineMesh.h"
#include "I4VideoDriver.h"
#include "I4Camera.h"
#include "I4Profile.h"

namespace i4graphics
{
	I4LineBatch::I4LineBatch(void)
		: mesh(nullptr)
	{
	}


	I4LineBatch::~I4LineBatch(void)
	{
		delete mesh;
	}

	bool I4LineBatch::create(int count)
	{
		mesh = new I4LineMesh;
		mesh->vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
		mesh->vertexBuffer->create(count*2, sizeof(I4Vertex_Pos_Col));
		vecSceneVertex.reserve(count);

		return true;
	}

	void I4LineBatch::draw()
	{
		I4PROFILE_THISFUNC;

		if (vecSceneVertex.size() > 0)
		{
			int totalCount = vecSceneVertex.size();
			int bufferCount = mesh->vertexBuffer->getCount();
			int srcStart = 0;

			while (totalCount > 0)
			{
				int srcCount = min(bufferCount, totalCount);
				mesh->vertexBuffer->copyFrom(&vecSceneVertex[srcStart], srcCount);
				mesh->bind();
				mesh->draw(srcCount, 0);
				mesh->unbind();

				totalCount -= srcCount;
				srcStart += srcCount;
			}			
		}

		vecSceneVertex.clear();
		vecCulledVertex.clear();
	}
	
}
