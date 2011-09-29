#pragma once

#include "i4graphics.h"

namespace i4graphics
{
	class I4Texture
	{
		friend class I4VideoDriver;
	public:
		virtual ~I4Texture()	{}

		virtual bool	load(const char* fname)		{ return true; }
				
	protected:
		I4Texture()			{}
	};
}