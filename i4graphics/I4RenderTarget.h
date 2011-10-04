#pragma once

#include "i4graphics.h"
#include "I4Format.h"

namespace i4graphics
{
	class I4GRAPHICS_API I4RenderTarget
	{
		friend class I4VideoDriver;
	public:
		virtual ~I4RenderTarget()	{}

		virtual bool	create(unsigned int width, unsigned int height, I4FORMAT format)		{ return true; }

	protected:
		I4RenderTarget()			{}
	};
}