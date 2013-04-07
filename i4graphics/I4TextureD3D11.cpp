#include "stdafx.h"
#include "I4TextureD3D11.h"
#include "DirectXTex.h"
#include "I4Log.h"
#include "I4StringUtil.h"

#pragma comment(lib, "DirectXTex.lib")

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

		DirectX::TexMetadata info;
		unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);

		wstring wfname;
		StringUtil::toWString(wfname, fname);

		wstring wpath;
		wstring wfilename;
		wstring wext;

		StringUtil::splitPath(wpath, wfilename, wext, wfname);

		wext = StringUtil::toLower(wext);
		if (wext.compare(L"dds") == 0)
		{
			if (FAILED(DirectX::LoadFromDDSFile(wfname.c_str(), DirectX::DDS_FLAGS_NONE, &info, *image)))
			{
				LOG_WARN << L"texture dds load failed. : " << wfname;
				return false;
			}
		}
		else if (wext.compare(L"tga") == 0)
		{
			if (FAILED(DirectX::LoadFromTGAFile(wfname.c_str(), &info, *image)))
			{
				LOG_WARN << L"texture tga load failed. : " << wfname;
				return false;
			}
		}
		else
		{
			if (FAILED(DirectX::LoadFromWICFile(wfname.c_str(), DirectX::DDS_FLAGS_NONE, &info, *image)))
			{
				LOG_WARN << L"texture etc load failed. : " << wfname;
				return false;
			}
		}

		if (FAILED(DirectX::CreateShaderResourceView(d3dDevice, image->GetImages(), image->GetImageCount(), info, &shaderResourceView)))
		{
			LOG_WARN << L"create share resource view failed. : " << wfname;
			return false;
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