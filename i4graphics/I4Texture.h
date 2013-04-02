#pragma once

#include "I4Hash.h"
using namespace i4core;

namespace i4graphics
{
	class Texture
	{
		friend class VideoDriver;
	public:
		virtual ~Texture()	{}

		virtual bool	load(const char* fname)	{ return true; }
	protected:
		Texture()			{}
	};
}