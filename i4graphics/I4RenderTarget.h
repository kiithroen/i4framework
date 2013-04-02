#pragma once

#include "I4Format.h"

namespace i4graphics
{
	class RenderTarget
	{
		friend class VideoDriver;
	public:
		virtual ~RenderTarget()	{}

		virtual bool	create(unsigned int width, unsigned int height, FORMAT format)														{ return true; }
		virtual bool	createDepthStencil(unsigned int width, unsigned int height, FORMAT texFormat, FORMAT dsvFormat, FORMAT srvFormat)	{ return true; }
	protected:
		RenderTarget()			{}
	};
}