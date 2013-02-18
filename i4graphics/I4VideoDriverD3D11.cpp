#include "stdafx.h"
#include "I4VideoDriverD3D11.h"
#include "I4Matrix4x4.h"
#include "I4GeometryBufferD3D11.h"
#include "I4ShaderProgramD3D11.h"
#include "I4TextureD3D11.h"
#include "I4RenderTargetD3D11.h"

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace i4graphics
{
	I4VideoDriverD3D11::I4VideoDriverD3D11()
		: d3dDevice(nullptr)
		, immediateContext(nullptr)
		, swapChain(nullptr)
		, backBufferRenderTargetView(nullptr)
		, backBufferDepthStencilTex(nullptr)
		, backBufferDepthStencilView(nullptr)
	{
		for (int i = 0; i < I4RASTERIZER_MODE_NUM; ++i)
		{
			rasterizerStates[i] = nullptr;
		}

		for (int i = 0; i < I4BLEND_MODE_NUM; ++i)
		{
			blendModes[i] = nullptr;
		}

		for (int i = 0; i < I4SAMPLER_STATE_NUM; ++i)
		{
			samplerStates[i] = nullptr;
		}
	}

	I4VideoDriverD3D11::~I4VideoDriverD3D11()
	{
		if (d3dDevice)	
		{
			immediateContext->ClearState();
		}

		if (backBufferDepthStencilTex) 
		{
			backBufferDepthStencilTex->Release();
			backBufferDepthStencilTex = nullptr;
		}

		if (backBufferDepthStencilView) 
		{
			backBufferDepthStencilView->Release();
			backBufferDepthStencilView = nullptr;

		}

		if (backBufferRenderTargetView) 
		{
			backBufferRenderTargetView->Release();
			backBufferRenderTargetView = nullptr;
		}

		if (swapChain) 
		{
			swapChain->Release();
			swapChain = nullptr;
		}

		if (immediateContext)
		{
			immediateContext->Release();
			immediateContext = nullptr;
		}

		if (d3dDevice) 
		{
			d3dDevice->Release();			
			d3dDevice = nullptr;
		}
		
		for (int i = 0; i < I4RASTERIZER_MODE_NUM; ++i)
		{
			if (rasterizerStates[i] != nullptr)
			{
				rasterizerStates[i]->Release();
				rasterizerStates[i] = nullptr;
			}
		}

		for (int i = 0; i < I4BLEND_MODE_NUM; ++i)
		{
			if (blendModes[i] != nullptr)
			{
				blendModes[i]->Release();
				blendModes[i] = nullptr;
			}
		}

		for (int i = 0; i < I4SAMPLER_STATE_NUM; ++i)
		{
			if (samplerStates[i] != nullptr)
			{
				samplerStates[i]->Release();
				samplerStates[i] = nullptr;
			}
		}
	}

	bool I4VideoDriverD3D11::initialize(void* windowID, unsigned int width, unsigned int height)
	{
		I4VideoDriver::initialize(windowID, width, height);

		HRESULT hr = S_OK;

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = (HWND)windowID;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE( featureLevels );

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
			driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(nullptr,
												driverTypes[driverTypeIndex],
												nullptr,
												createDeviceFlags,
												featureLevels,
												numFeatureLevels,
												D3D11_SDK_VERSION,
												&sd,
												&swapChain,
												&d3dDevice,
												&featureLevel,
												&immediateContext);
			if (SUCCEEDED(hr))
				break;
		}

		if (FAILED(hr))
			return false;

		ID3D11Texture2D* pBackBuffer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr))
			return false;

		hr = d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &backBufferRenderTargetView);
		pBackBuffer->Release();
		if (FAILED(hr))
			return false;

		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = d3dDevice->CreateTexture2D(&descDepth, nullptr, &backBufferDepthStencilTex);
		if (FAILED(hr))
			return false;

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = d3dDevice->CreateDepthStencilView(backBufferDepthStencilTex, &descDSV, &backBufferDepthStencilView);
		if (FAILED(hr))
			return false;

		immediateContext->OMSetRenderTargets(1, &backBufferRenderTargetView, backBufferDepthStencilView);
		setViewport(0, 0, width, height);

		D3D11_FILL_MODE fill[I4RASTERIZER_MODE_NUM] = 
		{ 
			D3D11_FILL_SOLID,
			D3D11_FILL_SOLID,
			D3D11_FILL_SOLID,
			D3D11_FILL_WIREFRAME,
			D3D11_FILL_WIREFRAME,
			D3D11_FILL_WIREFRAME
		};
		D3D11_CULL_MODE cull[I4RASTERIZER_MODE_NUM] = 
		{
			D3D11_CULL_NONE,
			D3D11_CULL_FRONT,
			D3D11_CULL_BACK,
			D3D11_CULL_NONE,
			D3D11_CULL_FRONT,
			D3D11_CULL_BACK
		};

		for (unsigned int i = 0; i < I4RASTERIZER_MODE_NUM; ++i)
		{
			D3D11_RASTERIZER_DESC rasterizerDesc;
			rasterizerDesc.FillMode = fill[i];
			rasterizerDesc.CullMode = cull[i];
			rasterizerDesc.FrontCounterClockwise = true;
			rasterizerDesc.DepthBias = false;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = true;
			rasterizerDesc.ScissorEnable = false;
			rasterizerDesc.MultisampleEnable = false;
			rasterizerDesc.AntialiasedLineEnable = false;
			hr = d3dDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerStates[i]);
			if (FAILED(hr))
				return false;
		}

		// default
		D3D11_BLEND_DESC BlendState;
		ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
		BlendState.AlphaToCoverageEnable = false;
		BlendState.IndependentBlendEnable = false;
		BlendState.RenderTarget[0].BlendEnable = false;
		BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = d3dDevice->CreateBlendState(&BlendState, &blendModes[I4BLEND_MODE_NONE]);
		if (FAILED(hr))
			return false;

		// alpha
		D3D11_BLEND_DESC alphaBlendDesc = {0};
		alphaBlendDesc.AlphaToCoverageEnable = false;
		alphaBlendDesc.IndependentBlendEnable = false;
		alphaBlendDesc.RenderTarget[0].BlendEnable = true;
		alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
		alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
		alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = d3dDevice->CreateBlendState(&alphaBlendDesc, &blendModes[I4BLEND_MODE_ALPHA]);
		if (FAILED(hr))
			return false;

		// add
		D3D11_BLEND_DESC addBlendDesc = {0};
		addBlendDesc.AlphaToCoverageEnable = false;
		addBlendDesc.IndependentBlendEnable = false;
		addBlendDesc.RenderTarget[0].BlendEnable = true;
		addBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		addBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		addBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		addBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		addBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		addBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		addBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = d3dDevice->CreateBlendState(&addBlendDesc, &blendModes[I4BLEND_MODE_ADD]);
		if (FAILED(hr))
			return false;

		D3D11_SAMPLER_DESC sampDescPoint;
		ZeroMemory( &sampDescPoint, sizeof(sampDescPoint) );
		sampDescPoint.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDescPoint.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDescPoint.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDescPoint.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDescPoint.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDescPoint.MinLOD = 0;
		sampDescPoint.MaxLOD = D3D11_FLOAT32_MAX;
		hr = d3dDevice->CreateSamplerState(&sampDescPoint, &samplerStates[I4SAMPLER_STATE_POINT]);
		if (FAILED(hr))
			return false;
		
		D3D11_SAMPLER_DESC sampDescLinear;
		ZeroMemory( &sampDescLinear, sizeof(sampDescLinear) );
		sampDescLinear.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDescLinear.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDescLinear.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDescLinear.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDescLinear.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDescLinear.MinLOD = 0;
		sampDescLinear.MaxLOD = D3D11_FLOAT32_MAX;
		hr = d3dDevice->CreateSamplerState(&sampDescLinear, &samplerStates[I4SAMPLER_STATE_LINEAR]);
		if (FAILED(hr))
			return false;

		setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);		
		setBlendMode(I4BLEND_MODE_NONE);

		return true;
	}

	bool I4VideoDriverD3D11::setupEnvironment()
	{
		if (I4VideoDriver::setupEnvironment() == false)
			return false;

		return true;
	}

	bool I4VideoDriverD3D11::beginScene()
	{
		return true;
	}

	void I4VideoDriverD3D11::endScene()
	{
		swapChain->Present(0, 0);
	}

	void I4VideoDriverD3D11::clearBackBuffer(unsigned char r, unsigned char g, unsigned char b)
	{
		float clearColor[4] = { (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, 1.0f };
		immediateContext->ClearRenderTargetView(backBufferRenderTargetView, clearColor);
		immediateContext->ClearDepthStencilView(backBufferDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void I4VideoDriverD3D11::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		immediateContext->RSSetViewports(1, &vp);
	}

	void I4VideoDriverD3D11::clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a)
	{
		float clearColor[4] = { r, g, b, a };
		immediateContext->ClearRenderTargetView(static_cast<I4RenderTargetD3D11*>(renderTarget)->getRenderTargetView(), clearColor);
	}

	void I4VideoDriverD3D11::clearDepthStencil(I4RenderTarget* renderTarget, float depth, unsigned char stencil)
	{
		immediateContext->ClearDepthStencilView(static_cast<I4RenderTargetD3D11*>(renderTarget)->getDepthStencilView(), D3D11_CLEAR_DEPTH, depth, stencil);
	}

	void I4VideoDriverD3D11::setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget)
	{
		ID3D11RenderTargetView* arrRTViews[8] = { 0, };

		for (unsigned int i = 0; i < num; ++i)
		{
			arrRTViews[i] = static_cast<I4RenderTargetD3D11*>(arrRenderTarget[i])->getRenderTargetView();
		}
		immediateContext->OMSetRenderTargets(num, arrRTViews, backBufferDepthStencilView);
	}

	void I4VideoDriverD3D11::setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget, I4RenderTarget* depthStencil)
	{
		ID3D11RenderTargetView* arrRTViews[8] = { 0, };

		for (unsigned int i = 0; i < num; ++i)
		{
			arrRTViews[i] = static_cast<I4RenderTargetD3D11*>(arrRenderTarget[i])->getRenderTargetView();
		}
		if (depthStencil)
		{
			immediateContext->OMSetRenderTargets(num, arrRTViews, static_cast<I4RenderTargetD3D11*>(depthStencil)->getDepthStencilView());
		}
		else
		{
			immediateContext->OMSetRenderTargets(num, arrRTViews, nullptr);
		}
	}

	void I4VideoDriverD3D11::resetRenderTarget()
	{
		immediateContext->OMSetRenderTargets(1, &backBufferRenderTargetView, backBufferDepthStencilView);
	}

	void I4VideoDriverD3D11::setRasterizerMode(I4RasterizerMode mode)
	{
		if (curRasterizerMode != mode)
		{
			I4VideoDriver::setRasterizerMode(mode);

			immediateContext->RSSetState(rasterizerStates[mode]);
		}
	}

	void I4VideoDriverD3D11::setBlendMode(I4BlendMode mode)
	{
		if (curBlendMode != mode)
		{
			I4VideoDriver::setBlendMode(mode);
			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			immediateContext->OMSetBlendState(blendModes[mode], blendFactor, 0xffffffff);
		}
	}

	I4ShaderProgram* I4VideoDriverD3D11::createShaderProgram()
	{
		return new I4ShaderProgramD3D11(d3dDevice, immediateContext, samplerStates);
	}

	I4VertexBuffer* I4VideoDriverD3D11::createVertexBuffer()
	{
		return new I4VertexBufferD3D11(d3dDevice, immediateContext);
	}

	I4IndexBuffer* I4VideoDriverD3D11::createIndexBuffer()
	{
		return new I4IndexBufferD3D11(d3dDevice, immediateContext);
	}

	I4ConstantBuffer* I4VideoDriverD3D11::createConstantBuffer()
	{
		return new I4ConstantBufferD3D11(d3dDevice);
	}

	I4Texture* I4VideoDriverD3D11::createTexture()
	{
		return new I4TextureD3D11(d3dDevice);
	}

	I4RenderTarget* I4VideoDriverD3D11::createRenderTarget()
	{
		return new I4RenderTargetD3D11(d3dDevice);
	}
}