#include "stdafx.h"
#include "I4PointLightMesh.h"
#include "I4VideoDriver.h"

namespace i4graphics
{
	I4PointLightMesh::I4PointLightMesh(void)
	{
	}


	I4PointLightMesh::~I4PointLightMesh(void)
	{
		destroy();
	}

	bool I4PointLightMesh::create(float r, unsigned short ringCount, unsigned short segmentCount)
	{	
		int vertexCount = (ringCount + 1)*(segmentCount + 1);
		vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
		if (vertexBuffer->create(vertexCount, sizeof(I4Vertex_Pos)) == false)
		{
			destroy();
			return false;
		}

		I4Vertex_Pos* vertices = nullptr;
		if (vertexBuffer->lock((void**)&vertices) == false)
		{
			destroy();
			return false;
		}
 
		int indexCount = 6*ringCount*(segmentCount + 1);
		indexBuffer = I4VideoDriver::getVideoDriver()->createIndexBuffer();
		if (indexBuffer->create(indexCount, sizeof(unsigned short)) == false)
		{
			destroy();
			return false;
		}

		unsigned short* indices = nullptr;
		if (indexBuffer->lock((void**)&indices) == false)
		{
			destroy();
			return false;
		}

		float deltaRingAngle = (I4PI/ringCount);
		float deltaSegAngle = (2*I4PI/segmentCount);

		unsigned short verticeIndex = 0 ;
 
		 for( int ring = 0; ring <= ringCount; ring++ )
		 {
			 float r0 = r * sinf (ring * deltaRingAngle);
			 float y0 = r * cosf (ring * deltaRingAngle);
 
			 for(int seg = 0; seg <= segmentCount; seg++)
			 {
				 float x0 = r0 * sinf(seg * deltaSegAngle);
				 float z0 = r0 * cosf(seg * deltaSegAngle);
 
				 vertices->x = x0;
				 vertices->y = y0;
				 vertices->z = z0;

				 vertices++;
 
				 if (ring != ringCount)
				 {
					 *indices++ = verticeIndex + segmentCount + 1;
					 *indices++ = verticeIndex;               
					 *indices++ = verticeIndex + segmentCount;
					 *indices++ = verticeIndex + segmentCount + 1;
					 *indices++ = verticeIndex + 1;
					 *indices++ = verticeIndex;
					 verticeIndex ++;
				 }
			 }
		 }
 
		 vertexBuffer->unlock();
		 indexBuffer->unlock();

		return true;
	}

	void I4PointLightMesh::destroy()
	{
		delete indexBuffer;
		delete vertexBuffer;
	}

	void I4PointLightMesh::bind()
	{
		vertexBuffer->bind();
		indexBuffer->bind();
	}

	void I4PointLightMesh::unbind()
	{
		indexBuffer->unbind();
		vertexBuffer->unbind();
	}

	void I4PointLightMesh::draw()
	{
		indexBuffer->draw(I4PT_TRIANGLELIST);
	}
}
