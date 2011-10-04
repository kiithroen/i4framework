#include "I4RenderTargetD3D10.h"
#include "I4Log.h"

namespace i4graphics
{
	I4RenderTargetD3D10::I4RenderTargetD3D10(ID3D10Device* d3dDevice)
		: d3dDevice(d3dDevice)
		, renderTargetView(NULL)
		, shaderResourceView(NULL)
	{	  	
	}

	I4RenderTargetD3D10::~I4RenderTargetD3D10()
	{
		unload();
	}

	bool I4RenderTargetD3D10::create(unsigned int width, unsigned int height, I4FORMAT format)
	{		
		D3D10_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Format = (DXGI_FORMAT)format;
		texDesc.Usage = D3D10_USAGE_DEFAULT;
		texDesc.BindFlags =  D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		if (FAILED(d3dDevice->CreateTexture2D(&texDesc, NULL, &renderTargetTex)))
		{
			I4LOG_WARN << L"render target texture create failed.";
			return false;
		}

		D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = texDesc.Format;
		rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		rtDesc.Texture2DArray.FirstArraySlice = 0;
		rtDesc.Texture2DArray.ArraySize = 1;
		rtDesc.Texture2DArray.MipSlice = 0;
		if (FAILED(d3dDevice->CreateRenderTargetView(renderTargetTex, &rtDesc, &renderTargetView)))
		{
			I4LOG_WARN << L"render target view create failed.";
			return false;
		}

		D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = 1;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		if (FAILED(d3dDevice->CreateShaderResourceView(renderTargetTex, &srvDesc, &shaderResourceView)))
		{
			I4LOG_WARN << L"shader resource view create failed.";
			return false;
		}

		return true;
	}

	void I4RenderTargetD3D10::unload()
	{
		if (shaderResourceView)
		{
			shaderResourceView->Release();
			shaderResourceView = NULL;
		}

		if (renderTargetView)
		{
			renderTargetView->Release();
			renderTargetView = NULL;
		}

		if (renderTargetTex)
		{
			renderTargetTex->Release();
			renderTargetTex = NULL;
		}
	}

}