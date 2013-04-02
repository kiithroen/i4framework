#include "stdafx.h"
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
	
	VertexBufferD3D11::VertexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context)
		: d3dDevice(device)
		, immediateContext(context)
		, vertexBuffer(nullptr)
	{
	}

	VertexBufferD3D11::~VertexBufferD3D11()
	{
		destroy();
	}

	bool VertexBufferD3D11::create(unsigned int count, unsigned int stride, const void* vertices)
	{
		if (VertexBuffer::create(count, stride, vertices) == false)
			return false;

		if (vertices != nullptr)
		{
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = stride*count;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
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
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;

			if (SUCCEEDED(d3dDevice->CreateBuffer(&bd, nullptr, &vertexBuffer)))
			{
				return true;
			}
		}

		LOG_WARN << L"vertext buffer create failed.";
		return false;
	}


	void VertexBufferD3D11::destroy()
	{
		if (vertexBuffer)
		{
			vertexBuffer->Release();
		}
	}

	bool VertexBufferD3D11::lock(void** data)
	{
		if (FAILED(immediateContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
		{
			LOG_WARN << L"vertext buffer lock failed.";
			return false;
		}

		*data = resource.pData;

		return true;
	}

	void VertexBufferD3D11::unlock()
	{
		immediateContext->Unmap(vertexBuffer, 0);
	}

	bool VertexBufferD3D11::copyFrom(const void* data)
	{
		return copyFrom(data, count);
	}

	bool VertexBufferD3D11::copyFrom(const void* data, unsigned int _count)
	{
		void* vertices;
		if (lock(&vertices) == false)
		{
			return false;
		}

		memcpy(vertices, data, _count*stride);
		unlock();

		return true;
	}

	void VertexBufferD3D11::bind()
	{
		UINT offset = 0;
		immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	}

	void VertexBufferD3D11::draw(PrimitiveType pt)
	{		
		immediateContext->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		immediateContext->Draw(count, 0);
	}

	void VertexBufferD3D11::draw(PrimitiveType pt, unsigned int _count, unsigned int _start)
	{
		immediateContext->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		immediateContext->Draw(_count, _start);
	}

	// -----------------------------------------------------------------------------------------------------------------

	IndexBufferD3D11::IndexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context)
		: d3dDevice(device)
		, immediateContext(context)
		, indexBuffer(nullptr)
	{

	}

	IndexBufferD3D11::~IndexBufferD3D11()
	{
		destroy();
	}

	bool IndexBufferD3D11::create(unsigned int count, unsigned int stride, const void* indices)
	{
		if (IndexBuffer::create(count, stride, indices) == false)
			return false;

		if (indices != nullptr)
		{
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = stride*count;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
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
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;
			
			if (SUCCEEDED(d3dDevice->CreateBuffer(&bd, nullptr, &indexBuffer)))
			{
				return true;
			}
		}		

		LOG_WARN << L"index buffer create failed.";
		return false;
	}

	void IndexBufferD3D11::destroy()
	{
		if (indexBuffer)
		{
			indexBuffer->Release();
			indexBuffer = nullptr;
		}
	}

	bool IndexBufferD3D11::lock(void **data)
	{
		if (FAILED(immediateContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
		{
			LOG_WARN << L"index buffer lock failed.";
			return false;
		}

		*data = resource.pData;


		return true;
	}

	void IndexBufferD3D11::unlock()
	{
		immediateContext->Unmap(indexBuffer, 0);
	}

	bool IndexBufferD3D11::copyFrom(const void* data)
	{
		return copyFrom(data, count);
	}

	bool IndexBufferD3D11::copyFrom(const void* data, unsigned int _count)
	{
		void* indices;
		if (lock(&indices) == false)
		{
			return false;
		}

		memcpy(indices, data, _count*stride);
		unlock();

		return true;
	}

	void IndexBufferD3D11::bind()
	{
		immediateContext->IASetIndexBuffer(indexBuffer, (DXGI_FORMAT)format, 0);
	}

	void IndexBufferD3D11::draw(PrimitiveType pt)
	{
		immediateContext->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		immediateContext->DrawIndexed(count, 0, 0);
	}

	void IndexBufferD3D11::draw(PrimitiveType pt, unsigned int _count, unsigned int _startIndex, int _baseVertex)
	{
		immediateContext->IASetPrimitiveTopology(PRIMITIVE_TYPE[pt]);
		immediateContext->DrawIndexed(_count, _startIndex, _baseVertex);
	}

	// -----------------------------------------------------------------------------------------------------------------

	ConstantBufferD3D11::ConstantBufferD3D11(ID3D11Device* device)
		: d3dDevice(device)
		, constantBuffer(nullptr)
	{

	}

	ConstantBufferD3D11::~ConstantBufferD3D11()
	{
		destroy();
	}

	bool ConstantBufferD3D11::create(unsigned int stride)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = stride;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		HRESULT hr = d3dDevice->CreateBuffer(&bd, nullptr, &constantBuffer);
		if (FAILED(hr))
		{
			LOG_WARN << L"can't create constant buffer";
			return false;
		}

		return true;
	}

	void ConstantBufferD3D11::destroy()
	{
		if (constantBuffer)
		{
			constantBuffer->Release();
			constantBuffer = nullptr;
		}
	}
}