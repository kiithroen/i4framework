#include "stdafx.h"
#include "I4PointLightMesh.h"
#include "I4VideoDriver.h"

namespace i4graphics
{
	PointLightMesh::PointLightMesh(void)
	{
	}


	PointLightMesh::~PointLightMesh(void)
	{
		destroy();
	}

	bool PointLightMesh::create(float r, unsigned short ringCount, unsigned short segmentCount)
	{	
		int vertexCount = (ringCount + 1)*(segmentCount + 1);
		vector<Vertex_Pos> vertices;
		vertices.resize(vertexCount);

		int indexCount = 6*ringCount*(segmentCount + 1);
		vector<unsigned short> indices;
		indices.resize(indexCount);

		float deltaRingAngle = (PI/ringCount);
		float deltaSegAngle = (2*PI/segmentCount);

		unsigned short verticeIndex = 0 ;

		Vertex_Pos* pV = &vertices[0];
		unsigned short* pI = &indices[0];

		for( int ring = 0; ring <= ringCount; ring++ )
		{
			float r0 = r * sinf (ring * deltaRingAngle);
			float y0 = r * cosf (ring * deltaRingAngle);

			for(int seg = 0; seg <= segmentCount; seg++)
			{
				float x0 = r0 * sinf(seg * deltaSegAngle);
				float z0 = r0 * cosf(seg * deltaSegAngle);

				pV->x = x0;
				pV->y = y0;
				pV->z = z0;

				++pV;

				if (ring != ringCount)
				{
					*pI++ = verticeIndex + segmentCount + 1;
					*pI++ = verticeIndex;               
					*pI++ = verticeIndex + segmentCount;
					*pI++ = verticeIndex + segmentCount + 1;
					*pI++ = verticeIndex + 1;
					*pI++ = verticeIndex;

					++verticeIndex;
				}
			}
		}

		vertexBuffer = VideoDriver::getVideoDriver()->createVertexBuffer();
		if (vertexBuffer->create(vertexCount, sizeof(Vertex_Pos), &vertices[0]) == false)
		{
			destroy();
			return false;
		}
 
		
		indexBuffer = VideoDriver::getVideoDriver()->createIndexBuffer();
		if (indexBuffer->create(indexCount, sizeof(unsigned short), &indices[0]) == false)
		{
			destroy();
			return false;
		}

		return true;
	}

	void PointLightMesh::destroy()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void PointLightMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void PointLightMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void PointLightMesh::draw()
	{
		indexBuffer->draw(PT_TRIANGLELIST);
	}
}
