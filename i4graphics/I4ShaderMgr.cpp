#include "I4ShaderMgr.h"
#include "I4ShaderProgram.h"
#include "I4VideoDriver.h"
#include "I4Log.h"

namespace i4graphics
{
	I4ShaderMgr::I4ShaderMgr()
		:activeShaderProgram(NULL)
	{
	}

	I4ShaderMgr::~I4ShaderMgr()
	{
		for (I4ShaderProgramMap::iterator itr = mapShaderProgram.begin(); itr != mapShaderProgram.end(); ++itr)
		{
			delete itr->second;
		}
		mapShaderProgram.clear();
	}


	bool I4ShaderMgr::load(const wchar_t* fname)
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

	bool I4ShaderMgr::begin(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		I4ShaderProgramMap::iterator itr = mapShaderProgram.find(mask);
		if (itr == mapShaderProgram.end())
		{
			activeShaderProgram = createShaderProgram(mask, inputElements, numElements);
			if (activeShaderProgram != NULL)
			{
				mapShaderProgram.insert(std::make_pair(mask, activeShaderProgram));
			}
			else
			{
				return false;
			}
		}
		else
		{
			activeShaderProgram = itr->second;
		}

		if (activeShaderProgram)
		{
			return activeShaderProgram->begin();
		}		

		return true;
	}

	void I4ShaderMgr::end()
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->end();
		}
	}

	void I4ShaderMgr::setVector(ShaderVector sv, float* v)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setVector(sv, v);
		}
	}

	void I4ShaderMgr::setVectorArray(ShaderVectorArray sva, float* v, unsigned int offset, unsigned int count)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setVectorArray(sva, v, offset, count);
		}
	}

	void I4ShaderMgr::setMatrix(ShaderMatrix sm, float* v)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setMatrix(sm, v);
		}
	}

	void I4ShaderMgr::setMatrixArray(ShaderMatrixArray sva, float* v, unsigned int offset, unsigned int count)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setMatrixArray(sva, v, offset, count);
		}
	}

	void I4ShaderMgr::setTexture(ShaderTexture st, I4Texture* tex)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setTexture(st, tex);
		}
	}

	
	I4ShaderProgram* I4ShaderMgr::createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
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

	//--------------------------------------------------------------------------------

	I4ShaderMgrMap I4ShaderMgr::mapShaderMgr;

	bool I4ShaderMgr::addShaderMgr(const std::wstring& fx)
	{
		I4ShaderMgr* shaderMgr = NULL;
		I4ShaderMgrMap::iterator itr = mapShaderMgr.find(fx);
		if (itr == mapShaderMgr.end())
		{
			shaderMgr = new I4ShaderMgr;
			if (shaderMgr->load(fx.c_str()) == false)
			{
				I4LOG_ERROR << L"shaderMgr load failed.(" << fx << L")";
				delete shaderMgr;
				return false;
			}
			mapShaderMgr.insert(std::make_pair(fx, shaderMgr));
		}
		else
		{
			I4LOG_WARN << L"shaderMgr already added.(" << fx << L")";
		}

		return true;
	}

	I4ShaderMgr* I4ShaderMgr::findShaderMgr(const std::wstring& fx)
	{
		I4ShaderMgrMap::iterator itr = mapShaderMgr.find(fx);
		if (itr != mapShaderMgr.end())
		{
			return itr->second;
		}

		I4LOG_ERROR << L"can't find shader mgr.(" << fx << L")";
		return NULL;
	}

	void I4ShaderMgr::destroyShaderMgr()
	{
		for (I4ShaderMgrMap::iterator itr = mapShaderMgr.begin(); itr != mapShaderMgr.end(); ++itr)
		{
			delete itr->second;
		}
		mapShaderMgr.clear();
	}
}