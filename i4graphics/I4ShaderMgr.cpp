#include "stdafx.h"
#include "I4ShaderMgr.h"
#include "I4Shader.h"
#include "I4VideoDriver.h"
#include "I4Log.h"
#include "I4Profile.h"

namespace i4graphics
{
	ShaderMgr::ShaderMgr()
		:activeShader(nullptr)
	{
	}

	ShaderMgr::~ShaderMgr()
	{
		for (auto& itr : mapShader)
		{
			delete itr.second;
		}
		mapShader.clear();
	}


	bool ShaderMgr::load(const char* fname)
	{
		ifstream ifs;

		ifs.open(fname, ifstream::in);

		if (ifs.is_open() == false)
		{
			LOG_WARN << L"can't open file. :" << fname;
			return false;
		}

		ifs.seekg(0, ios_base::end);

		int size = (int)ifs.tellg();
		if (size <= 0)
		{
			LOG_WARN << L"incorrect file. : " << fname;
			return false;
		}

		ifs.seekg(0, ios::beg);
		char* buffer = new char[size + 1];
		memset(buffer, 0, size + 1);
		ifs.read(buffer, size);

		if (strlen(buffer) == 0)
		{
			LOG_WARN << L"%s is empty\n" << fname;
			delete[] buffer;
			return false;
		}

		baseShaderCode = buffer;

		delete[] buffer;

		return true;
	}

	bool ShaderMgr::begin(unsigned int mask, const INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		auto itr = mapShader.find(mask);
		if (itr == mapShader.end())
		{
			activeShader = createShader(mask, inputElements, numElements);
			if (activeShader != nullptr)
			{
				mapShader.insert(make_pair(mask, activeShader));
			}
			else
			{
				return false;
			}
		}
		else
		{
			activeShader = itr->second;
		}

		if (activeShader)
		{
			return activeShader->begin();
		}		

		return true;
	}

	void ShaderMgr::end()
	{
		if (activeShader)
		{
			activeShader->end();
		}
	}
	
	void ShaderMgr::setConstantBuffer(ShaderType type, unsigned int slot, ConstantBuffer* constantBuffer, void* data)
	{
		if (activeShader)
		{
			activeShader->setConstantBuffer(type, slot, constantBuffer, data);
		}
	}

	void ShaderMgr::setTexture(unsigned int slot, const Texture* tex)
	{
		if (activeShader)
		{
			activeShader->setTexture(slot, tex);
		}
	}

	void ShaderMgr::setRenderTarget(unsigned int slot, const RenderTarget* tex)
	{
		if (activeShader)
		{
			activeShader->setRenderTarget(slot, tex);
		}
	}

	void ShaderMgr::setSamplerState(unsigned int slot, SamplerState state)
	{
		if (activeShader)
		{
			activeShader->setSamplerState(slot, state);
		}
	}

	Shader* ShaderMgr::createShader(unsigned int mask, const INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		PROFILE_THISFUNC;

		string finalShaderCode = "";

		if (mask & SHADER_MASK_TEX_DIFFUSE)
		{
			finalShaderCode += "#define MASK_TEX_DIFFUSE\r\n";
		}

		if (mask & SHADER_MASK_TEX_SPECULAR)
		{
			finalShaderCode += "#define MASK_TEX_SPECULAR\r\n";
		}

		if (mask & SHADER_MASK_TEX_NORMAL)
		{
			finalShaderCode += "#define MASK_TEX_NORMAL\r\n";
		}
	
		if (mask & SHADER_MASK_SKINNING)
		{
			finalShaderCode += "#define MASK_SKINNING\r\n";
		}

		finalShaderCode += baseShaderCode;

		Shader* shaderProgram = VideoDriver::getVideoDriver()->createShader();
		if (shaderProgram->createFromString(finalShaderCode.c_str(), inputElements, numElements) == false)
		{
			delete shaderProgram;
			return nullptr;
		}

		return shaderProgram;
	}

	//--------------------------------------------------------------------------------

	ShaderMgrMap ShaderMgr::mapShaderMgr;

	bool ShaderMgr::addShaderMgr(const string& fxName)
	{
		ShaderMgr* shaderMgr = nullptr;
		auto itr = mapShaderMgr.find(fxName);
		if (itr == mapShaderMgr.end())
		{
			shaderMgr = new ShaderMgr;
			if (shaderMgr->load(fxName.c_str()) == false)
			{
				LOG_WARN << L"shaderMgr load failed.(" << fxName.c_str() << L")";
				delete shaderMgr;
				shaderMgr = nullptr;
			}
			mapShaderMgr.insert(make_pair(fxName, shaderMgr));
		}
		else
		{
			LOG_WARN << L"shaderMgr already added.(" << fxName.c_str() << L")";
		}

		return true;
	}

	ShaderMgr* ShaderMgr::findShaderMgr(const string& fxName)
	{
		auto itr = mapShaderMgr.find(fxName);
		if (itr != mapShaderMgr.end())
		{
			return itr->second;
		}

		LOG_WARN << L"can't find shader mgr.(" << fxName.c_str() << L")";
		return nullptr;
	}

	void ShaderMgr::destroyShaderMgr()
	{
		for (auto& itr : mapShaderMgr)
		{
			delete itr.second;
		}
		mapShaderMgr.clear();
	}
}