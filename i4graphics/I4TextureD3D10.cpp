#include "I4TextureD3D10.h"
#include "I4Log.h"

namespace i4graphics
{
	I4TextureD3D10::I4TextureD3D10(ID3D10Device* d3dDevice)
		: d3dDevice(d3dDevice)
		, shaderResourceView(NULL)
	{
	}

	I4TextureD3D10::~I4TextureD3D10()
	{
		unload();
	}

	bool I4TextureD3D10::load(const char* fname)
	{
		if (FAILED(D3DX10CreateShaderResourceViewFromFileA(d3dDevice, fname, NULL, NULL, &shaderResourceView, NULL)))
		{
			I4LOG_WARN << L"texture load failed. : " << fname;
			return false;
		}

		return true;
	}

	void I4TextureD3D10::unload()
	{
		if (shaderResourceView)
		{
			shaderResourceView->Release();
			shaderResourceView = NULL;
		}
	}

}