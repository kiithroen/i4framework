#include "I4ShaderMgr.h"
#include "I4ShaderProgram.h"
#include "I4VideoDriver.h"
#include "I4UberShaderMgr.h"
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


	bool I4ShaderMgr::initialize(const wchar_t* fname)
	{
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

	//--------------------------------------------------------------------------------

	I4ShaderMgr* I4ShaderMgr::shaderMgr = NULL;

	I4ShaderProgram* I4ShaderMgr::createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		return I4VideoDriver::getVideoDriver()->createShaderProgram();
	}

	void I4ShaderMgr::createShaderMgr(I4ShaderMgrType type)
	{
		switch (type)
		{
		case I4SHADER_MGR_TYPE_NULL:
			shaderMgr = new I4ShaderMgr;
			break;
		case I4SHADER_MGR_TYPE_UBER:
			shaderMgr = new I4UberShaderMgr;
			break;
		default:
			I4LOG_ERROR << "Undefined Shader Type : " << type;
			assert(false);
			shaderMgr = new I4ShaderMgr;
			break;
		}
	}

	void I4ShaderMgr::destroyShaderMgr()
	{
		delete shaderMgr;
		shaderMgr = NULL;
	}
}