#include "stdafx.h"
#include "I4TextureD3D11.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "I4Log.h"
#include "I4StringUtil.h"

namespace i4graphics
{
	TextureD3D11::TextureD3D11(ID3D11Device* _d3dDevice, ID3D11DeviceContext* _d3dContext)
		: d3dDevice(_d3dDevice)
		, d3dContext(_d3dContext)
		, shaderResourceView(nullptr)
	{
	}

	TextureD3D11::~TextureD3D11()
	{
		unload();
	}

	bool TextureD3D11::load(const char* fname)
	{
		if (Texture::load(fname) == false)
			return false;

		wstring wfname;
		to_wstring(wfname, fname);
		if (FAILED(DirectX::CreateDDSTextureFromFile(d3dDevice, wfname.c_str(), nullptr, &shaderResourceView)))
		{
			LOG_WARN << L"texture is not dds. : " << wfname;

			if (FAILED(DirectX::CreateWICTextureFromFile(d3dDevice, d3dContext, wfname.c_str(), nullptr, &shaderResourceView)))
			{
				LOG_WARN << L"texture load failed. : " << wfname;
				return false;
			}
		}

		return true;
	}

	void TextureD3D11::unload()
	{
		if (shaderResourceView)
		{
			shaderResourceView->Release();
			shaderResourceView = nullptr;
		}
	}

}