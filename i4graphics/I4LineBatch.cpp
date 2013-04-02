#include "stdafx.h"
#include "I4LineBatch.h"
#include "I4LineMesh.h"
#include "I4VideoDriver.h"
#include "I4Camera.h"
#include "I4Profile.h"

namespace i4graphics
{
	LineBatch::LineBatch(void)
		: mesh(nullptr)
	{
	}


	LineBatch::~LineBatch(void)
	{
		delete mesh;
	}

	bool LineBatch::create(int count)
	{
		mesh = new LineMesh;
		mesh->vertexBuffer = VideoDriver::getVideoDriver()->createVertexBuffer();
		mesh->vertexBuffer->create(count*2, sizeof(Vertex_Pos_Col));
		vecSceneVertex.reserve(count);

		return true;
	}

	void LineBatch::draw()
	{
		PROFILE_THISFUNC;

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
