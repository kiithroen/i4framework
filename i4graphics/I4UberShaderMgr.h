#pragma once

#include "i4graphics.h"
#include "I4ShaderMgr.h"

namespace i4graphics
{

	class I4UberShaderMgr : public I4ShaderMgr
	{
	public:
		I4UberShaderMgr();
		virtual ~I4UberShaderMgr();

		virtual bool	initialize(const wchar_t* fname);
		
	private:
		virtual I4ShaderProgram*	createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);
	};

}
