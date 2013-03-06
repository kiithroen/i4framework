#include "stdafx.h"
#include "I4GeometryBuffer.h"
#include "I4Log.h"

namespace i4graphics
{

	I4VertexBuffer::I4VertexBuffer()
		: count(0)
		, stride(0)
	{

	}


	I4VertexBuffer::~I4VertexBuffer()
	{

	}

	bool I4VertexBuffer::create(unsigned int _count, unsigned int _stride, const void* vertices)
	{
		count = _count;
		stride = _stride;

		return true;
	}

	bool I4VertexBuffer::copyFrom(const void* data)
	{
		return true;
	}

	bool I4VertexBuffer::copyFrom(const void* data, unsigned int count)
	{
		return true;
	}

	bool I4VertexBuffer::lock(void** data)
	{
		return true;
	}

	void I4VertexBuffer::unlock()
	{

	}

	void I4VertexBuffer::bind()
	{

	}

	void I4VertexBuffer::unbind()
	{

	}

	void I4VertexBuffer::draw(I4PrimitiveType pt)
	{

	}

	void I4VertexBuffer::draw(I4PrimitiveType pt, unsigned int count, unsigned int start)
	{

	}

//-------------------------------------------------------------------------------------

	I4IndexBuffer::I4IndexBuffer()
		: count(0)
		, stride(0)
	{

	}

	I4IndexBuffer::~I4IndexBuffer()
	{

	}

	bool I4IndexBuffer::create(unsigned int _count, unsigned int _stride, const void* indices)
	{
		count = _count;
		stride = _stride;

		if (stride == sizeof(unsigned int))
		{
			format = I4FORMAT_R32_UINT;
		}
		else if (stride == sizeof(unsigned short))
		{
			format = I4FORMAT_R16_UINT;
		}
		else
		{
			I4LOG_WARN << L"unsupported format";
			return false;
		}

		return true;
	}

	bool I4IndexBuffer::copyFrom(const void* data)
	{
		return true;
	}

	bool I4IndexBuffer::copyFrom(const void* data, unsigned int count)
	{
		return true;
	}

	bool I4IndexBuffer::lock(void** data)
	{
		return true;
	}

	void I4IndexBuffer::unlock()
	{

	}

	void I4IndexBuffer::bind()
	{

	}

	void I4IndexBuffer::unbind()
	{

	}

	void I4IndexBuffer::draw(I4PrimitiveType pt)
	{

	}

	void I4IndexBuffer::draw(I4PrimitiveType pt, unsigned int _count, unsigned int _startIndex, int _baseVertex)
	{

	}


	//-------------------------------------------------------------------------------------

	I4ConstantBuffer::I4ConstantBuffer()
	{

	}

	I4ConstantBuffer::~I4ConstantBuffer()
	{

	}

	bool I4ConstantBuffer::create(unsigned int stride)
	{
		return true;
	}
}