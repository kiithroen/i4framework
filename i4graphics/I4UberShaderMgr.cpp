#include "I4UberShaderMgr.h"
#include "I4VideoDriver.h"
#include "I4ShaderProgram.h"
#include "I4Log.h"

namespace i4graphics
{
	I4UberShaderMgr::I4UberShaderMgr()
	{
	}

	I4UberShaderMgr::~I4UberShaderMgr()
	{
	}

	bool I4UberShaderMgr::initialize(const wchar_t* fname)
	{
		std::ifstream ifs;

		ifs.open(fname, std::ifstream::in);

		if (ifs.is_open() == false)
		{
			I4LOG_WARN << L"can't open file. :" << fname;
			return false;
		}

		ifs.seekg(0, std::ios_base::end);

		int size = (int)ifs.tellg();
		if (size <= 0)
		{
			I4LOG_WARN << L"incorrect file. : " << fname;
			return false;
		}

		ifs.seekg(0, std::ios::beg);
		char* buffer = new char[size + 1];
		memset(buffer, 0, size + 1);
		ifs.read(buffer, size);

		if (strlen(buffer) == 0)
		{
			I4LOG_WARN << L"%s is empty\n" << fname;
			delete[] buffer;
			return false;
		}

		baseShaderCode = buffer;

		delete[] buffer;

		return true;
	}

	I4ShaderProgram* I4UberShaderMgr::createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		std::string finalShaderCode = "";

		if (mask & I4SHADER_MASK_VERTEX_COLOR)
		{
			finalShaderCode += "#define VERTEX_COLOR_MASK\r\n";
		}
	
		finalShaderCode += baseShaderCode;

		I4ShaderProgram* shaderProgram = I4VideoDriver::getVideoDriver()->createShaderProgram();
		if (shaderProgram->createFromString(finalShaderCode.c_str(), inputElements, numElements) == false)
		{
			delete shaderProgram;
			return NULL;
		}

		return shaderProgram;
	}
}