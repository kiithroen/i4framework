#include "stdafx.h"
#include "I4ShaderMgr.h"
#include "I4ShaderProgram.h"
#include "I4VideoDriver.h"
#include "I4Log.h"

namespace i4graphics
{
	I4ShaderMgr::I4ShaderMgr()
		:activeShaderProgram(nullptr)
	{
	}

	I4ShaderMgr::~I4ShaderMgr()
	{
		for (auto&itr : mapShaderProgram)
		{
			delete itr.second;
		}
		mapShaderProgram.clear();
	}


	bool I4ShaderMgr::load(const char* fname)
	{
		ifstream ifs;

		ifs.open(fname, ifstream::in);

		if (ifs.is_open() == false)
		{
			I4LOG_WARN << L"can't open file. :" << fname;
			return false;
		}

		ifs.seekg(0, ios_base::end);

		int size = (int)ifs.tellg();
		if (size <= 0)
		{
			I4LOG_WARN << L"incorrect file. : " << fname;
			return false;
		}

		ifs.seekg(0, ios::beg);
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
		auto itr = mapShaderProgram.find(mask);
		if (itr == mapShaderProgram.end())
		{
			activeShaderProgram = createShaderProgram(mask, inputElements, numElements);
			if (activeShaderProgram != nullptr)
			{
				mapShaderProgram.insert(make_pair(mask, activeShaderProgram));
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
	
	void I4ShaderMgr::setConstantBuffer(I4ShaderProgramType type, unsigned int slot, I4ConstantBuffer* constantBuffer, void* data)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setConstantBuffer(type, slot, constantBuffer, data);
		}
	}

	void I4ShaderMgr::setTexture(unsigned int slot, const I4Texture* tex)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setTexture(slot, tex);
		}
	}

	void I4ShaderMgr::setRenderTarget(unsigned int slot, const I4RenderTarget* tex)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setRenderTarget(slot, tex);
		}
	}

	void I4ShaderMgr::setSamplerState(unsigned int slot, I4SamplerState state)
	{
		if (activeShaderProgram)
		{
			activeShaderProgram->setSamplerState(slot, state);
		}
	}

	I4ShaderProgram* I4ShaderMgr::createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		string finalShaderCode = "";

		if (mask & I4SHADER_MASK_TEX_DIFFUSE)
		{
			finalShaderCode += "#define MASK_TEX_DIFFUSE\r\n";
		}

		if (mask & I4SHADER_MASK_TEX_SPECULAR)
		{
			finalShaderCode += "#define MASK_TEX_SPECULAR\r\n";
		}

		if (mask & I4SHADER_MASK_TEX_NORMAL)
		{
			finalShaderCode += "#define MASK_TEX_NORMAL\r\n";
		}
	
		if (mask & I4SHADER_MASK_SKINNING)
		{
			finalShaderCode += "#define MASK_SKINNING\r\n";
		}

		finalShaderCode += baseShaderCode;

		I4ShaderProgram* shaderProgram = I4VideoDriver::getVideoDriver()->createShaderProgram();
		if (shaderProgram->createFromString(finalShaderCode.c_str(), inputElements, numElements) == false)
		{
			delete shaderProgram;
			return nullptr;
		}

		return shaderProgram;
	}

	//--------------------------------------------------------------------------------

	I4ShaderMgrMap I4ShaderMgr::mapShaderMgr;

	bool I4ShaderMgr::addShaderMgr(const string& fxName)
	{
		I4ShaderMgr* shaderMgr = nullptr;
		auto itr = mapShaderMgr.find(fxName);
		if (itr == mapShaderMgr.end())
		{
			shaderMgr = new I4ShaderMgr;
			if (shaderMgr->load(fxName.c_str()) == false)
			{
				I4LOG_WARN << L"shaderMgr load failed.(" << fxName.c_str() << L")";
				delete shaderMgr;
				shaderMgr = nullptr;
			}
			mapShaderMgr.insert(make_pair(fxName, shaderMgr));
		}
		else
		{
			I4LOG_WARN << L"shaderMgr already added.(" << fxName.c_str() << L")";
		}

		return true;
	}

	I4ShaderMgr* I4ShaderMgr::findShaderMgr(const string& fxName)
	{
		auto itr = mapShaderMgr.find(fxName);
		if (itr != mapShaderMgr.end())
		{
			return itr->second;
		}

		I4LOG_WARN << L"can't find shader mgr.(" << fxName.c_str() << L")";
		return nullptr;
	}

	void I4ShaderMgr::destroyShaderMgr()
	{
		for (auto&itr : mapShaderMgr)
		{
			delete itr.second;
		}
		mapShaderMgr.clear();
	}
}