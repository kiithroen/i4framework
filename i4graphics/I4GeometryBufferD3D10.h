#pragma once

#include "i4graphics.h"
#include "I4VideoDriverD3D10.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	class I4VertexBufferD3D10 : public I4VertexBuffer
	{
	public:
		I4VertexBufferD3D10(ID3D10Device* device);
		virtual ~I4VertexBufferD3D10();

		virtual bool	create(unsigned int count, unsigned int stride, const void* vertices = 0) override;
		void			destroy();

		virtual bool	copyFrom(const void* data) override;

		virtual bool	lock(void** data) override;
		virtual void	unlock() override;

		virtual void	bind() override;

		virtual void	draw(I4PrimitiveType type) override;

	private:
		ID3D10Device*				d3dDevice;
		ID3D10Buffer*				vertexBuffer;
		D3D10_PRIMITIVE_TOPOLOGY	topology;
	};

	// -----------------------------------------------------------------------------------------------------------------

	class I4IndexBufferD3D10 : public I4IndexBuffer
	{
	public:
		I4IndexBufferD3D10(ID3D10Device* device);
		~I4IndexBufferD3D10();

		virtual bool	create(unsigned int count, unsigned int stride, const void* indices = 0) override;	
		void			destroy();

		virtual bool	copyFrom(const void* data) override;

		virtual bool	lock(void** data) override;
		virtual void	unlock() override;

		virtual void	bind() override;

		virtual void	draw(I4PrimitiveType pt) override;

	private:
		ID3D10Device*		d3dDevice;
		ID3D10Buffer*		indexBuffer;
	};

}