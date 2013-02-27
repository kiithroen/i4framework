#pragma once

#include "I4VideoDriverD3D11.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	class I4VertexBufferD3D11 : public I4VertexBuffer
	{
	public:
		I4VertexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context);
		virtual ~I4VertexBufferD3D11();

		virtual bool	create(unsigned int count, unsigned int stride, const void* vertices = 0) override;
		void			destroy();

		virtual bool	copyFrom(const void* data) override;

		virtual bool	lock(void** data) override;
		virtual void	unlock() override;

		virtual void	bind() override;

		virtual void	draw(I4PrimitiveType type) override;

	private:
		ID3D11Device*				d3dDevice;
		ID3D11DeviceContext*		immediateContext;
		ID3D11Buffer*				vertexBuffer;		
		D3D11_MAPPED_SUBRESOURCE	resource;
	};

	// ---------------------------------------------------------------------------------------------

	class I4IndexBufferD3D11 : public I4IndexBuffer
	{
	public:
		I4IndexBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* context);
		~I4IndexBufferD3D11();

		virtual bool	create(unsigned int count, unsigned int stride, const void* indices = 0) override;	
		void			destroy();

		virtual bool	copyFrom(const void* data) override;

		virtual bool	lock(void** data) override;
		virtual void	unlock() override;

		virtual void	bind() override;

		virtual void	draw(I4PrimitiveType pt) override;

	private:
		ID3D11Device*				d3dDevice;
		ID3D11DeviceContext*		immediateContext;
		D3D11_MAPPED_SUBRESOURCE	resource;
		ID3D11Buffer*				indexBuffer;
	};

	class I4ConstantBufferD3D11 : public I4ConstantBuffer
	{
	public:
		I4ConstantBufferD3D11(ID3D11Device* device);
		~I4ConstantBufferD3D11();

		virtual bool	create(unsigned int stride) override;	
		void			destroy();

		ID3D11Buffer*	getBuffer() const	{ return constantBuffer; }

	private:
		ID3D11Device*				d3dDevice;
		ID3D11Buffer*				constantBuffer;
	};
}