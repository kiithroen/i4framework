#pragma once

#include "I4VideoDriverD3D11.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	class VertexBufferD3D11 : public VertexBuffer
	{
	public:
		VertexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context);
		virtual ~VertexBufferD3D11();

		virtual bool	create(unsigned int count, unsigned int stride, const void* vertices = 0) override;
		void			destroy();

		virtual bool	copyFrom(const void* data) override;
		virtual bool	copyFrom(const void* data, unsigned int _count) override;

		virtual bool	lock(void** data) override;
		virtual void	unlock() override;

		virtual void	bind() override;

		virtual void	draw(PrimitiveType type) override;
		virtual void	draw(PrimitiveType pt, unsigned int _count, unsigned int _start);

	private:
		ID3D11Device*				d3dDevice;
		ID3D11DeviceContext*		immediateContext;
		ID3D11Buffer*				vertexBuffer;		
		D3D11_MAPPED_SUBRESOURCE	resource;
	};

	// ---------------------------------------------------------------------------------------------

	class IndexBufferD3D11 : public IndexBuffer
	{
	public:
		IndexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context);
		~IndexBufferD3D11();

		virtual bool	create(unsigned int count, unsigned int stride, const void* indices = 0) override;	
		void			destroy();

		virtual bool	copyFrom(const void* data) override;
		virtual bool	copyFrom(const void* data, unsigned int _count) override;

		virtual bool	lock(void** data) override;
		virtual void	unlock() override;

		virtual void	bind() override;

		virtual void	draw(PrimitiveType pt) override;
		virtual void	draw(PrimitiveType pt, unsigned int _count, unsigned int _startIndex, int _baseVertex);

	private:
		ID3D11Device*				d3dDevice;
		ID3D11DeviceContext*		immediateContext;
		D3D11_MAPPED_SUBRESOURCE	resource;
		ID3D11Buffer*				indexBuffer;
	};

	class ConstantBufferD3D11 : public ConstantBuffer
	{
	public:
		ConstantBufferD3D11(ID3D11Device* device);
		~ConstantBufferD3D11();

		virtual bool	create(unsigned int stride) override;	
		void			destroy();

		ID3D11Buffer*	getBuffer() const	{ return constantBuffer; }

	private:
		ID3D11Device*				d3dDevice;
		ID3D11Buffer*				constantBuffer;
	};
}