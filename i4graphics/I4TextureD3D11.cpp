#include "stdafx.h"
#include "I4TextureD3D11.h"
#include "DDSTextureLoader.h"
#include "I4Log.h"
#include "I4StringUtil.h"

namespace i4graphics
{
	I4TextureD3D11::I4TextureD3D11(ID3D11Device* d3dDevice)
		: d3dDevice(d3dDevice)
		, shaderResourceView(nullptr)
	{
	}

	I4TextureD3D11::~I4TextureD3D11()
	{
		unload();
	}

	bool I4TextureD3D11::load(const char* fname)
	{
		if (I4Texture::load(fname) == false)
			return false;

		wstring wfname;
		to_wstring(wfname, fname);
		if (FAILED(CreateDDSTextureFromFile(d3dDevice, wfname.c_str(), nullptr, &shaderResourceView)))
		{
			I4LOG_WARN << L"texture load failed. : " << wfname;
			return false;
		}

		return true;
	}

	void I4TextureD3D11::unload()
	{
		if (shaderResourceView)
		{
			shaderResourceView->Release();
			shaderResourceView = nullptr;
		}
	}

}