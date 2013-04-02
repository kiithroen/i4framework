#pragma once

#include "I4VideoDriver.h"

#include <D3D11.h>

namespace i4graphics
{
	class VideoDriverD3D11 : public VideoDriver
	{
		typedef map<string, ID3D11Buffer*>	ConstantBufferMap;
		ConstantBufferMap		constantBufferMap;
		friend class VideoDriver;
	public:
		~VideoDriverD3D11();

		virtual VideoDriverMode	getVideoDriverMode() const override	{ return VIDEO_DRIVER_MODE_D3D11; }
		
		virtual bool				initialize(void* windowID, unsigned int width, unsigned int height) override;
		virtual bool				setupEnvironment() override;
		
		virtual bool				beginScene() override;
		virtual void				endScene() override;

		virtual void				clearBackBuffer(unsigned char r, unsigned char g, unsigned char b) override;
		virtual void				clearRenderTarget(RenderTarget* renderTarget, float r, float g, float b, float a) override;
		virtual void				clearDepthStencil(RenderTarget* renderTarget, float depth, unsigned char stencil) override;
		virtual void				setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
		virtual void				resetViewport() override;

		virtual void				setRenderTarget(unsigned int num, RenderTarget** arrRenderTarget) override;
		virtual void				setRenderTarget(unsigned int num, RenderTarget** arrRenderTarget, RenderTarget* depthStencil) override;
		virtual void				setRenderTargetDepthStencil(RenderTarget* depthStencil) override;
		virtual void				resetBackBufferRenderTarget(bool enableDepthStencil) override;

		virtual void				setRasterizerMode(RasterizerMode mode) override;
		virtual void				setDepthStencilMode(DepthStencilMode mode) override;
		virtual void				setBlendMode(BlendMode mode) override;

		virtual Shader*	createShader() override;

		virtual VertexBuffer*		createVertexBuffer() override;
		virtual IndexBuffer*		createIndexBuffer() override;
		virtual ConstantBuffer*	createConstantBuffer() override;

		virtual Texture*			createTexture() override;

		virtual RenderTarget*		createRenderTarget() override;

	private:
		VideoDriverD3D11();
	private:
		ID3D11Device*				d3dDevice;
		ID3D11DeviceContext*		immediateContext;
		IDXGISwapChain*				swapChain;
		ID3D11RenderTargetView*		backBufferRenderTargetView;
		ID3D11Texture2D*			backBufferDepthStencilTex;
		ID3D11DepthStencilView*		backBufferDepthStencilView;
		ID3D11RasterizerState*		rasterizerStates[RASTERIZER_MODE_NUM];
		ID3D11DepthStencilState*    depthStencilStates[DEPTHSTENCIL_MODE_NUM];
		ID3D11BlendState*			blendModes[BLEND_MODE_NUM];		
		ID3D11SamplerState*			samplerStates[SAMPLER_STATE_NUM];
		D3D_DRIVER_TYPE				driverType;
		D3D_FEATURE_LEVEL			featureLevel;
	};

}