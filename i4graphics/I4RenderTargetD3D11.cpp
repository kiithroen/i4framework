#include "stdafx.h"
#include "I4RenderTargetD3D11.h"
#include "I4Log.h"
using namespace i4core;

namespace i4graphics
{
	I4RenderTargetD3D11::I4RenderTargetD3D11(ID3D11Device* d3dDevice)
		: d3dDevice(d3dDevice)
		, renderTargetView(nullptr)
		, depthStencilView(nullptr)
		, shaderResourceView(nullptr)
	{	  	
	}

	I4RenderTargetD3D11::~I4RenderTargetD3D11()
	{
		unload();
	}

	bool I4RenderTargetD3D11::create(unsigned int width, unsigned int height, I4FORMAT format)
	{		
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Format = (DXGI_FORMAT)format;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags =  D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		if (FAILED(d3dDevice->CreateTexture2D(&texDesc, nullptr, &renderTargetTex)))
		{
			I4LOG_WARN << L"render target texture create failed.";
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = texDesc.Format;
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtDesc.Texture2DArray.FirstArraySlice = 0;
		rtDesc.Texture2DArray.ArraySize = 1;
		rtDesc.Texture2DArray.MipSlice = 0;
		if (FAILED(d3dDevice->CreateRenderTargetView(renderTargetTex, &rtDesc, &renderTargetView)))
		{
			I4LOG_WARN << L"render target view create failed.";
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
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

	bool I4RenderTargetD3D11::createDepthStencil(unsigned int width, unsigned int height, I4FORMAT texFormat, I4FORMAT dsvFormat, I4FORMAT srvFormat)
	{		
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Format = (DXGI_FORMAT)texFormat;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags =  D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		if (FAILED(d3dDevice->CreateTexture2D(&texDesc, nullptr, &renderTargetTex)))
		{
			I4LOG_WARN << L"render target texture create failed.";
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(dsvDesc));
		dsvDesc.Format = (DXGI_FORMAT)dsvFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		if (FAILED(d3dDevice->CreateDepthStencilView(renderTargetTex, &dsvDesc, &depthStencilView)))
		{
			I4LOG_WARN << L"render target view create failed.";
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = (DXGI_FORMAT)srvFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		if (FAILED(d3dDevice->CreateShaderResourceView(renderTargetTex, &srvDesc, &shaderResourceView)))
		{
			I4LOG_WARN << L"shader resource view create failed.";
			return false;
		}

		return true;
	}

	void I4RenderTargetD3D11::unload()
	{
		if (shaderResourceView)
		{
			shaderResourceView->Release();
			shaderResourceView = nullptr;
		}

		if (depthStencilView)
		{
			depthStencilView->Release();
			depthStencilView = nullptr;
		}

		if (renderTargetView)
		{
			renderTargetView->Release();
			renderTargetView = nullptr;
		}

		if (renderTargetTex)
		{
			renderTargetTex->Release();
			renderTargetTex = nullptr;
		}
	}

}