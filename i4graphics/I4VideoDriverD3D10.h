#pragma once

#include "i4graphics.h"
#include "I4VideoDriver.h"

#include <d3d10.h>
#include <d3dx10.h>

namespace i4graphics
{
	class I4VideoDriverD3D10 : public I4VideoDriver
	{
		friend class I4VideoDriver;
	public:
		~I4VideoDriverD3D10();

		virtual I4VideoDriverMode	getVideoDriverMode() const override	{ return I4VIDEO_DRIVER_MODE_D3D10; }
		
		virtual bool				initialize(void* windowID, unsigned int width, unsigned int height) override;
		virtual bool				setupEnvironment() override;
		
		virtual bool				beginScene() override;
		virtual void				endScene() override;

		virtual void				clearBackBuffer(unsigned char r, unsigned char g, unsigned char b) override;
		virtual void				clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a) override;

		virtual void				setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		virtual void				setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget, bool isDepthStencil) override;
		virtual void				resetRenderTarget() override;

		virtual void				setRasterizerMode(I4RasterizerMode mode) override;
		virtual void				setBlendMode(I4BlendMode mode) override;

		virtual I4ShaderProgram*	createShaderProgram() override;

		virtual I4VertexBuffer*		createVertexBuffer() override;
		virtual I4IndexBuffer*		createIndexBuffer() override;

		virtual I4Texture*			createTexture() override;

		virtual I4RenderTarget*		createRenderTarget() override;

	private:
		I4VideoDriverD3D10();

	private:
		D3D10_DRIVER_TYPE			driverType;
		ID3D10Device*				d3dDevice;
		IDXGISwapChain*				swapChain;
		ID3D10RenderTargetView*		backBufferRenderTargetView;
		ID3D10Texture2D*			depthStencilTex;
		ID3D10DepthStencilView*		depthStencilView;
		ID3D10RasterizerState*		rasterizerStates[I4RASTERIZER_MODE_NUM];
		ID3D10BlendState*			blendStates[I4BLEND_MODE_NUM];
	};

}