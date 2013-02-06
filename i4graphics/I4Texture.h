#pragma once

#include "i4graphics.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics
{
	class I4Texture : public I4HashData<I4Texture>
	{
		friend class I4VideoDriver;
	public:
		virtual ~I4Texture()	{}

		virtual bool	load(const char* fname)	{ return true; }
	protected:
		I4Texture()			{}
	};
}