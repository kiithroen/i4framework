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

	void I4ShaderMgr::apply()
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->apply();
		}
	}

	void I4ShaderMgr::setBool(I4ShaderBool sb, bool v)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setBool(sb, v);
		}
	}

	void I4ShaderMgr::setInt(I4ShaderInt si, int v)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setInt(si, v);
		}
	}

	void I4ShaderMgr::setFloat(I4ShaderFloat sf, float v)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setFloat(sf, v);
		}
	}

	void I4ShaderMgr::setVector(I4ShaderVector sv, float* v)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setVector(sv, v);
		}
	}

	void I4ShaderMgr::setVectorArray(I4ShaderVectorArray sva, float* v, unsigned int offset, unsigned int count)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setVectorArray(sva, v, offset, count);
		}
	}

	void I4ShaderMgr::setMatrix(I4ShaderMatrix sm, float* v)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setMatrix(sm, v);
		}
	}

	void I4ShaderMgr::setMatrixArray(I4ShaderMatrixArray sva, float* v, unsigned int offset, unsigned int count)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setMatrixArray(sva, v, offset, count);
		}
	}

	void I4ShaderMgr::setTexture(unsigned int stage, I4Texture* tex)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setTexture(stage, tex);
		}
	}

	void I4ShaderMgr::setRenderTarget(unsigned int stage, I4RenderTarget* rt)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setRenderTarget(stage, rt);
		}
	}
	
	void I4ShaderMgr::setTexture(I4ShaderTexture st, I4Texture* tex)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setTexture(st, tex);
		}
	}

	void I4ShaderMgr::setRenderTarget(I4ShaderRenderTarget srt, I4RenderTarget* rt)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setRenderTarget(srt, rt);
		}
	}

	I4ShaderProgram* I4ShaderMgr::createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		std::string finalShaderCode = "";

		if (mask & I4SHADER_MASK_DIFFUSEMAP)
		{
			finalShaderCode += "#define MASK_DIFFUSEMAP\r\n";
		}

		if (mask & I4SHADER_MASK_SPECULARMAP)
		{
			finalShaderCode += "#define MASK_SPECULARMAP\r\n";
		}

		if (mask & I4SHADER_MASK_NORMALMAP)
		{
			finalShaderCode += "#define MASK_NORMALMAP\r\n";
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
				I4LOG_WARN << L"shaderMgr load failed.(" << fx << L")";
				delete shaderMgr;
				shaderMgr = NULL;
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

		I4LOG_WARN << L"can't find shader mgr.(" << fx << L")";
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