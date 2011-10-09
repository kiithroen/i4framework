#include "I4VideoDriverD3D10.h"
#include "I4Matrix4x4.h"
#include "I4GeometryBufferD3D10.h"
#include "I4ShaderProgramD3D10.h"
#include "I4TextureD3D10.h"
#include "I4RenderTargetD3D10.h"

#pragma comment(lib, "d3d10.lib")

#ifdef _DEBUG
#pragma comment(lib, "d3dx10d.lib")
#else
#pragma comment(lib, "d3dx10.lib")
#endif

#define USE_NVPERFHUD

namespace i4graphics
{
	I4VideoDriverD3D10::I4VideoDriverD3D10()
		: driverType(D3D10_DRIVER_TYPE_NULL)
		, d3dDevice(NULL)
		, swapChain(NULL)
		, renderTargetView(NULL)
		, depthStencilTex(NULL)
		, depthStencilView(NULL)
	{
	}

	I4VideoDriverD3D10::~I4VideoDriverD3D10()
	{
		if (d3dDevice)	
		{
			d3dDevice->ClearState();
		}

		if (depthStencilTex) 
		{
			depthStencilTex->Release();
			depthStencilTex = NULL;
		}

		if (depthStencilView) 
		{
			depthStencilView->Release();
			depthStencilView = NULL;

		}

		if (renderTargetView) 
		{
			renderTargetView->Release();
			renderTargetView = NULL;
		}

		if (swapChain) 
		{
			swapChain->Release();
			swapChain = NULL;
		}

		if (d3dDevice) 
		{
			d3dDevice->Release();			
			d3dDevice = NULL;
		}
	}

	bool I4VideoDriverD3D10::initialize(void* windowID, unsigned int width, unsigned int height)
	{
		I4VideoDriver::initialize(windowID, width, height);

		HRESULT hr = S_OK;

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

		D3D10_DRIVER_TYPE driverTypes[] =
		{
			D3D10_DRIVER_TYPE_HARDWARE,
			D3D10_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = sizeof(driverTypes)/sizeof(driverTypes[0]);

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

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
			driverType = driverTypes[driverTypeIndex];
			hr = D3D10CreateDeviceAndSwapChain( NULL, driverType, NULL, createDeviceFlags,
				D3D10_SDK_VERSION, &sd, &swapChain, &d3dDevice );
			if (SUCCEEDED(hr))
				break;
		}

		if (FAILED(hr))
			return false;

		ID3D10Texture2D* pBuffer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBuffer);
		if (FAILED(hr))
			return false;

		hr = d3dDevice->CreateRenderTargetView(pBuffer, NULL, &renderTargetView);
		pBuffer->Release();
		if (FAILED(hr))
			return false;

		D3D10_TEXTURE2D_DESC descDepth;
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D10_USAGE_DEFAULT;
		descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = d3dDevice->CreateTexture2D(&descDepth, NULL, &depthStencilTex);
		if (FAILED(hr))
			return false;

		D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = d3dDevice->CreateDepthStencilView(depthStencilTex, &descDSV, &depthStencilView);
		if (FAILED(hr))
			return false;

		d3dDevice->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

		setViewport(0, 0, width, height);

		return true;
	}

	bool I4VideoDriverD3D10::setupEnvironment()
	{
		if (I4VideoDriver::setupEnvironment() == false)
			return false;

		return true;
	}

	bool I4VideoDriverD3D10::beginScene()
	{
		return true;
	}

	void I4VideoDriverD3D10::endScene()
	{
		swapChain->Present(0, 0);
	}

	void I4VideoDriverD3D10::clearScreen(unsigned char r, unsigned char g, unsigned char b)
	{
		float clearColor[4] = { (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, 1.0f };
		d3dDevice->ClearRenderTargetView(renderTargetView, clearColor);
		d3dDevice->ClearDepthStencilView(depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
	}

	void I4VideoDriverD3D10::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		D3D10_VIEWPORT vp;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		d3dDevice->RSSetViewports(1, &vp);
	}

	void I4VideoDriverD3D10::clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a)
	{
		float clearColor[4] = { r, g, b, a };
		d3dDevice->ClearRenderTargetView(static_cast<I4RenderTargetD3D10*>(renderTarget)->getRenderTargetView(), clearColor);
	}

	void I4VideoDriverD3D10::setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget)
	{
		ID3D10RenderTargetView* arrRTViews[8] = { 0, };

		for (unsigned int i = 0; i < num; ++i)
		{
			arrRTViews[i] = static_cast<I4RenderTargetD3D10*>(arrRenderTarget[i])->getRenderTargetView();
		}
		d3dDevice->OMSetRenderTargets(num, arrRTViews, NULL);
	}

	void I4VideoDriverD3D10::resetRenderTarget()
	{
		d3dDevice->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	}

	I4ShaderProgram* I4VideoDriverD3D10::createShaderProgram()
	{
		return new I4ShaderProgramD3D10(d3dDevice);
	}

	I4VertexBuffer* I4VideoDriverD3D10::createVertexBuffer()
	{
		return new I4VertexBufferD3D10(d3dDevice);
	}

	I4IndexBuffer* I4VideoDriverD3D10::createIndexBuffer()
	{
		return new I4IndexBufferD3D10(d3dDevice);
	}

	I4Texture* I4VideoDriverD3D10::createTexture()
	{
		return new I4TextureD3D10(d3dDevice);
	}

	I4RenderTarget* I4VideoDriverD3D10::createRenderTarget()
	{
		return new I4RenderTargetD3D10(d3dDevice);
	}
}