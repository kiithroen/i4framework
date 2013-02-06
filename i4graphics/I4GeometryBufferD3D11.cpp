#include "I4GeometryBufferD3D11.h"
#include "I4Log.h"

namespace i4graphics
{
	static D3D_PRIMITIVE_TOPOLOGY PRIMITIVE_TYPE[] =
	{
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};
	
	I4VertexBufferD3D11::I4VertexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context)
		: d3dDevice(device)
		, immediateContext(context)
		, vertexBuffer(nullptr)
	{
	}

	I4VertexBufferD3D11::~I4VertexBufferD3D11()
	{
		destroy();
	}

	bool I4VertexBufferD3D11::create(unsigned int count, unsigned int stride, const void* vertices)
	{
		if (I4VertexBuffer::create(count, stride, vertices) == false)
			return false;

		if (vertices != nullptr)
		{
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = stride*count;
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;

			if (SUCCEEDED(d3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffer)))
			{
				return true;
			}
		}
		else
		{
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = stride*count;
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;

			if (SUCCEEDED(d3dDevice->CreateBuffer(&bd, nullptr, &vertexBuffer)))
			{
				return true;
			}
		}

		I4LOG_WARN << L"vertext buffer create failed.";
		return false;
	}


	void I4VertexBufferD3D11::destroy()
	{
		if (vertexBuffer)
		{
			vertexBuffer->Release();
		}
	}

	bool I4VertexBufferD3D11::lock(void** data)
	{
		if (FAILED(immediateContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
		{
			I4LOG_WARN << L"vertext buffer lock failed.";
			return false;
		}

		*data = resource.pData;

		return true;
	}

	void I4VertexBufferD3D11::unlock()
	{
		immediateContext->Unmap(vertexBuffer, 0);
	}

	bool I4VertexBufferD3D11::copyFrom(const void* data)
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

	void I4VertexBufferD3D11::bind()
	{
		UINT offset = 0;
		immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	}

	void I4VertexBufferD3D11::draw(I4PrimitiveType pt)
	{		
		immediateContext->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		immediateContext->Draw(count, 0);
	}

	// -----------------------------------------------------------------------------------------------------------------

	I4IndexBufferD3D11::I4IndexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context)
		: d3dDevice(device)
		, immediateContext(context)
		, indexBuffer(nullptr)
	{

	}

	I4IndexBufferD3D11::~I4IndexBufferD3D11()
	{
		destroy();
	}

	bool I4IndexBufferD3D11::create(unsigned int count, unsigned int stride, const void* indices)
	{
		if (I4IndexBuffer::create(count, stride, indices) == false)
			return false;

		if (indices != nullptr)
		{
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = stride*count;
			bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = indices;
		
			if (SUCCEEDED(d3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer)))
			{
				return true;
			}
		}
		else
		{
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = stride*count;
			bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = indices;

			if (SUCCEEDED(d3dDevice->CreateBuffer(&bd, nullptr, &indexBuffer)))
			{
				return true;
			}
		}		

		I4LOG_WARN << L"index buffer create failed.";
		return false;
	}

	void I4IndexBufferD3D11::destroy()
	{
		if (indexBuffer)
		{
			indexBuffer->Release();
			indexBuffer = nullptr;
		}
	}

	bool I4IndexBufferD3D11::lock(void **data)
	{
		if (FAILED(immediateContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
		{
			I4LOG_WARN << L"index buffer lock failed.";
			return false;
		}

		*data = resource.pData;


		return true;
	}

	void I4IndexBufferD3D11::unlock()
	{
		immediateContext->Unmap(indexBuffer, 0);
	}

	bool I4IndexBufferD3D11::copyFrom(const void* data)
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

	void I4IndexBufferD3D11::bind()
	{
		immediateContext->IASetIndexBuffer(indexBuffer, (DXGI_FORMAT)format, 0);
	}

	void I4IndexBufferD3D11::draw(I4PrimitiveType pt)
	{
		immediateContext->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		immediateContext->DrawIndexed(count, 0, 0);
	}

}