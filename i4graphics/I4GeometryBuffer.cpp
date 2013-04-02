#include "stdafx.h"
#include "I4GeometryBuffer.h"
#include "I4Log.h"

namespace i4graphics
{

	VertexBuffer::VertexBuffer()
		: count(0)
		, stride(0)
	{

	}


	VertexBuffer::~VertexBuffer()
	{

	}

	bool VertexBuffer::create(unsigned int _count, unsigned int _stride, const void* vertices)
	{
		count = _count;
		stride = _stride;

		return true;
	}

	bool VertexBuffer::copyFrom(const void* data)
	{
		return true;
	}

	bool VertexBuffer::copyFrom(const void* data, unsigned int count)
	{
		return true;
	}

	bool VertexBuffer::lock(void** data)
	{
		return true;
	}

	void VertexBuffer::unlock()
	{

	}

	void VertexBuffer::bind()
	{

	}

	void VertexBuffer::unbind()
	{

	}

	void VertexBuffer::draw(PrimitiveType pt)
	{

	}

	void VertexBuffer::draw(PrimitiveType pt, unsigned int count, unsigned int start)
	{

	}

//-------------------------------------------------------------------------------------

	IndexBuffer::IndexBuffer()
		: count(0)
		, stride(0)
	{

	}

	IndexBuffer::~IndexBuffer()
	{

	}

	bool IndexBuffer::create(unsigned int _count, unsigned int _stride, const void* indices)
	{
		count = _count;
		stride = _stride;

		if (stride == sizeof(unsigned int))
		{
			format = FORMAT_R32_UINT;
		}
		else if (stride == sizeof(unsigned short))
		{
			format = FORMAT_R16_UINT;
		}
		else
		{
			LOG_WARN << L"unsupported format";
			return false;
		}

		return true;
	}

	bool IndexBuffer::copyFrom(const void* data)
	{
		return true;
	}

	bool IndexBuffer::copyFrom(const void* data, unsigned int count)
	{
		return true;
	}

	bool IndexBuffer::lock(void** data)
	{
		return true;
	}

	void IndexBuffer::unlock()
	{

	}

	void IndexBuffer::bind()
	{

	}

	void IndexBuffer::unbind()
	{

	}

	void IndexBuffer::draw(PrimitiveType pt)
	{

	}

	void IndexBuffer::draw(PrimitiveType pt, unsigned int _count, unsigned int _startIndex, int _baseVertex)
	{

	}


	//-------------------------------------------------------------------------------------

	ConstantBuffer::ConstantBuffer()
	{

	}

	ConstantBuffer::~ConstantBuffer()
	{

	}

	bool ConstantBuffer::create(unsigned int stride)
	{
		return true;
	}
}