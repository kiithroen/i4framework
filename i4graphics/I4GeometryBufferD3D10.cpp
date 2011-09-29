#include "I4GeometryBufferD3D10.h"
#include "I4Log.h"

namespace i4graphics
{
	static D3D_PRIMITIVE_TOPOLOGY PRIMITIVE_TYPE[] =
	{
		D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};
	
	I4VertexBufferD3D10::I4VertexBufferD3D10(ID3D10Device* device)
		: d3dDevice(device)
		, vertexBuffer(NULL)
		, topology(D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
	{
	}

	I4VertexBufferD3D10::~I4VertexBufferD3D10()
	{
		destroy();
	}

	bool I4VertexBufferD3D10::create(unsigned int count, unsigned int stride, const void* vertices)
	{
		I4VertexBuffer::create(count, stride, vertices);

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = stride*count;
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;

		if (FAILED(d3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffer)))
		{
			I4LOG_WARN << L"vertext buffer create failed.";
			return false;
		}

		return true;
	}


	void I4VertexBufferD3D10::destroy()
	{
		if (vertexBuffer)
		{
			vertexBuffer->Release();
		}
	}

	bool I4VertexBufferD3D10::lock(void** data)
	{
		if (FAILED(vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, data)))
		{
			I4LOG_WARN << L"vertext buffer lock failed.";
			return false;
		}

		return true;
	}

	void I4VertexBufferD3D10::unlock()
	{
		vertexBuffer->Unmap();
	}

	bool I4VertexBufferD3D10::copyFrom(const void* data)
	{
		void* vertices;
		if (lock(&vertices) == false)
		{
			return false;
		}

		memcpy(vertices, data, count*stride);
		unlock();

		return true;
	}

	void I4VertexBufferD3D10::bind()
	{
		UINT offset = 0;
		d3dDevice->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	}

	void I4VertexBufferD3D10::draw(I4PrimitiveType pt)
	{		
		d3dDevice->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		d3dDevice->Draw(count, 0);
	}

	// -----------------------------------------------------------------------------------------------------------------

	I4IndexBufferD3D10::I4IndexBufferD3D10(ID3D10Device* device)
		: d3dDevice(device)
		, indexBuffer(NULL)
	{

	}

	I4IndexBufferD3D10::~I4IndexBufferD3D10()
	{
		destroy();
	}

	bool I4IndexBufferD3D10::create(unsigned int count, unsigned int stride, const void* indices)
	{
		I4IndexBuffer::create(count, stride, indices);

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(DWORD)*count;
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indices;
		if (FAILED(d3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer)))
		{
			I4LOG_WARN << L"index buffer create failed.";
			return false;
		}

		return true;
	}

	void I4IndexBufferD3D10::destroy()
	{
		if (indexBuffer)
		{
			indexBuffer->Release();
			indexBuffer = NULL;
		}
	}

	bool I4IndexBufferD3D10::lock(void **data)
	{
		if (FAILED(indexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, data)))
		{
			I4LOG_WARN << L"index buffer lock failed.";
			return false;
		}

		return true;
	}

	void I4IndexBufferD3D10::unlock()
	{
		indexBuffer->Unmap();
	}

	bool I4IndexBufferD3D10::copyFrom(const void* data)
	{
		void* indices;
		if (lock(&indices) == false)
		{
			return false;
		}

		memcpy(indices, data, count*stride);
		unlock();

		return true;
	}

	void I4IndexBufferD3D10::bind()
	{
		d3dDevice->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void I4IndexBufferD3D10::draw(I4PrimitiveType pt, unsigned int verticeCount)
	{
		d3dDevice->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		d3dDevice->DrawIndexed(count, 0, 0);
	}

}