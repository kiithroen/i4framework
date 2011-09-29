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

		virtual I4VideoDriverMode	getVideoDriverMode() const	{ return I4VIDEO_DRIVER_MODE_D3D10; }
		
		virtual bool				initialize(void* windowID, unsigned int width, unsigned int height);
		virtual bool				setupEnvironment();
		
		virtual bool				beginScene();
		virtual void				endScene();

		virtual void				clearScreen(unsigned char r, unsigned char g, unsigned char b);

		virtual void				setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		virtual I4ShaderProgram*	createShaderProgram();

		virtual I4VertexBuffer*		createVertexBuffer();
		virtual I4IndexBuffer*		createIndexBuffer();

		virtual I4Texture*			createTexture();

	private:
		I4VideoDriverD3D10();

	private:
		D3D10_DRIVER_TYPE			driverType;
		ID3D10Device*				d3dDevice;
		IDXGISwapChain*				swapChain;
		ID3D10RenderTargetView*		renderTargetView;
		ID3D10Texture2D*			depthStencil;
		ID3D10DepthStencilView*		depthStencilView;
	};

}