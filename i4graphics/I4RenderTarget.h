#pragma once

#include "i4graphics.h"
#include "I4Format.h"

namespace i4graphics
{
	class I4RenderTarget
	{
		friend class I4VideoDriver;
	public:
		virtual ~I4RenderTarget()	{}

		virtual bool	create(unsigned int width, unsigned int height, I4FORMAT format)														{ return true; }
		virtual bool	createDepthStencil(unsigned int width, unsigned int height, I4FORMAT texFormat, I4FORMAT dsvFormat, I4FORMAT srvFormat)	{ return true; }
	protected:
		I4RenderTarget()			{}
	};
}