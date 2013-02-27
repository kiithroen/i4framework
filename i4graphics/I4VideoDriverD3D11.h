#pragma once

#include "i4graphics.h"
#include "I4VideoDriver.h"

#include <D3D11.h>

namespace i4graphics
{
	class I4VideoDriverD3D11 : public I4VideoDriver
	{
		typedef map<string, ID3D11Buffer*>	ConstantBufferMap;
		ConstantBufferMap		constantBufferMap;
		friend class I4VideoDriver;
	public:
		~I4VideoDriverD3D11();

		virtual I4VideoDriverMode	getVideoDriverMode() const override	{ return I4VIDEO_DRIVER_MODE_D3D11; }
		
		virtual bool				initialize(void* windowID, unsigned int width, unsigned int height) override;
		virtual bool				setupEnvironment() override;
		
		virtual bool				beginScene() override;
		virtual void				endScene() override;

		virtual void				clearBackBuffer(unsigned char r, unsigned char g, unsigned char b) override;
		virtual void				clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a) override;
		virtual void				clearDepthStencil(I4RenderTarget* renderTarget, float depth, unsigned char stencil) override;
		virtual void				setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
		virtual void				resetViewport() override;

		virtual void				setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget) override;
		virtual void				setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget, I4RenderTarget* depthStencil) override;
		virtual void				setRenderTargetDepthStencil(I4RenderTarget* depthStencil) override;
		virtual void				resetRenderTarget() override;

		virtual void				setRasterizerMode(I4RasterizerMode mode) override;
		virtual void				setBlendMode(I4BlendMode mode) override;

		virtual I4Shader*	createShader() override;

		virtual I4VertexBuffer*		createVertexBuffer() override;
		virtual I4IndexBuffer*		createIndexBuffer() override;
		virtual I4ConstantBuffer*	createConstantBuffer() override;

		virtual I4Texture*			createTexture() override;

		virtual I4RenderTarget*		createRenderTarget() override;

	private:
		I4VideoDriverD3D11();

	private:
		ID3D11Device*				d3dDevice;
		ID3D11DeviceContext*		immediateContext;
		IDXGISwapChain*				swapChain;
		ID3D11RenderTargetView*		backBufferRenderTargetView;
		ID3D11Texture2D*			backBufferDepthStencilTex;
		ID3D11DepthStencilView*		backBufferDepthStencilView;
		ID3D11RasterizerState*		rasterizerStates[I4RASTERIZER_MODE_NUM];
		ID3D11BlendState*			blendModes[I4BLEND_MODE_NUM];		
		ID3D11SamplerState*			samplerStates[I4SAMPLER_STATE_NUM];
		D3D_DRIVER_TYPE				driverType;
		D3D_FEATURE_LEVEL			featureLevel;
	};

}