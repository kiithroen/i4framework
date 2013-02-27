#include "stdafx.h"
#include "I4VideoDriver.h"
#include "I4VideoDriverD3D11.h"
#include "I4Shader.h"
#include "I4GeometryBuffer.h"
#include "I4Texture.h"
#include "I4TextureMgr.h"
#include "I4RenderTarget.h"
#include "I4MathUtil.h"
#include "I4Log.h"

namespace i4graphics
{
	I4VideoDriver::I4VideoDriver()
		: width(0)
		, height(0)
		, curRasterizerMode(I4RASTERIZER_MODE_INVALID)
		, curBlendMode(I4BLEND_MODE_INVALID)
		, textureMgr(nullptr)
	{
	}

	I4VideoDriver::~I4VideoDriver()
	{
	}

	bool I4VideoDriver::initialize(void* _windowID, unsigned int _width, unsigned int _height)
	{
		windowID = _windowID;
		width	= _width;
		height	= _height;

		textureMgr = new I4TextureMgr;

		return true;
	}

	void I4VideoDriver::finalize()
	{
		delete textureMgr;
	}

	bool I4VideoDriver::setupEnvironment()
	{
		setViewport(0, 0, width, height);

		return true;
	}

	bool I4VideoDriver::beginScene()
	{
		return true;
	}

	void I4VideoDriver::endScene()
	{

	}

	void I4VideoDriver::clearBackBuffer(unsigned char r, unsigned char g, unsigned char b)
	{
	}

	void I4VideoDriver::clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a)
	{
	}

	void I4VideoDriver::clearDepthStencil(I4RenderTarget* renderTarget, float depth, unsigned char stencil)
	{
	}

	void I4VideoDriver::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
	}

	void I4VideoDriver::resetViewport()
	{
	}

	void I4VideoDriver::setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget)
	{
	}

	void I4VideoDriver::setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget, I4RenderTarget* depthStencil)
	{
	}

	void I4VideoDriver::setRenderTargetDepthStencil(I4RenderTarget* depthStencil)
	{
	}

	void I4VideoDriver::resetRenderTarget()
	{
	}

	void I4VideoDriver::setRasterizerMode(I4RasterizerMode mode)
	{
		curRasterizerMode = mode;
	}

	void I4VideoDriver::setBlendMode(I4BlendMode mode)
	{
		curBlendMode = mode;
	}
	
	I4Shader* I4VideoDriver::createShader()
	{
		return new I4Shader;
	}

	I4VertexBuffer* I4VideoDriver::createVertexBuffer()
	{
		return new I4VertexBuffer;
	}

	I4IndexBuffer* I4VideoDriver::createIndexBuffer()
	{
		return new I4IndexBuffer;
	}

	I4ConstantBuffer* I4VideoDriver::createConstantBuffer()
	{
		return new I4ConstantBuffer;
	}

	I4Texture* I4VideoDriver::createTexture()
	{
		return new I4Texture;
	}

	I4RenderTarget* I4VideoDriver::createRenderTarget()
	{
		return new I4RenderTarget;
	}

	//------------------------------------------------------------

	I4VideoDriver* I4VideoDriver::videoDriver = nullptr;

	void I4VideoDriver::createVideoDriver(I4VideoDriverMode mode)
	{
		assert(videoDriver == nullptr);

		switch (mode)
		{
		case I4VIDEO_DRIVER_MODE_NULL:
			videoDriver = new I4VideoDriver;
			break;
		case I4VIDEO_DRIVER_MODE_D3D11:
			videoDriver = new I4VideoDriverD3D11;
			break;
		default:
			I4LOG_WARN << "Undefined Video Driver Mode : " << mode;
			videoDriver = new I4VideoDriver;
			break;
		}
	}

	void I4VideoDriver::destroyVideoDriver()
	{
		if (videoDriver)
		{
			videoDriver->finalize();
			delete videoDriver;
			videoDriver = nullptr;
		}
	}
}