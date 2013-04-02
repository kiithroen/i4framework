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
	VideoDriver::VideoDriver()
		: width(0)
		, height(0)
		, curRasterizerMode(RASTERIZER_MODE_INVALID)
		, curDepthStencilMode(DEPTHSTENCIL_MODE_INVAILD)
		, curBlendMode(BLEND_MODE_INVALID)
		, textureMgr(nullptr)
	{
	}

	VideoDriver::~VideoDriver()
	{
	}

	bool VideoDriver::initialize(void* _windowID, unsigned int _width, unsigned int _height)
	{
		windowID = _windowID;
		width	= _width;
		height	= _height;

		textureMgr = new TextureMgr;

		return true;
	}

	void VideoDriver::finalize()
	{
		delete textureMgr;
	}

	bool VideoDriver::setupEnvironment()
	{
		setViewport(0, 0, width, height);

		return true;
	}

	bool VideoDriver::beginScene()
	{
		return true;
	}

	void VideoDriver::endScene()
	{

	}

	void VideoDriver::clearBackBuffer(unsigned char r, unsigned char g, unsigned char b)
	{
	}

	void VideoDriver::clearRenderTarget(RenderTarget* renderTarget, float r, float g, float b, float a)
	{
	}

	void VideoDriver::clearDepthStencil(RenderTarget* renderTarget, float depth, unsigned char stencil)
	{
	}

	void VideoDriver::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
	}

	void VideoDriver::resetViewport()
	{
	}

	void VideoDriver::setRenderTarget(unsigned int num, RenderTarget** arrRenderTarget)
	{
	}

	void VideoDriver::setRenderTarget(unsigned int num, RenderTarget** arrRenderTarget, RenderTarget* depthStencil)
	{
	}

	void VideoDriver::setRenderTargetDepthStencil(RenderTarget* depthStencil)
	{
	}

	void VideoDriver::resetBackBufferRenderTarget(bool enableDepthStencil)
	{
	}

	void VideoDriver::setRasterizerMode(RasterizerMode mode)
	{
		curRasterizerMode = mode;
	}

	void VideoDriver::setDepthStencilMode(DepthStencilMode mode)
	{
		curDepthStencilMode = mode;
	}

	void VideoDriver::setBlendMode(BlendMode mode)
	{
		curBlendMode = mode;
	}
	
	Shader* VideoDriver::createShader()
	{
		return new Shader;
	}

	VertexBuffer* VideoDriver::createVertexBuffer()
	{
		return new VertexBuffer;
	}

	IndexBuffer* VideoDriver::createIndexBuffer()
	{
		return new IndexBuffer;
	}

	ConstantBuffer* VideoDriver::createConstantBuffer()
	{
		return new ConstantBuffer;
	}

	Texture* VideoDriver::createTexture()
	{
		return new Texture;
	}

	RenderTarget* VideoDriver::createRenderTarget()
	{
		return new RenderTarget;
	}

	//------------------------------------------------------------

	VideoDriver* VideoDriver::videoDriver = nullptr;

	void VideoDriver::createVideoDriver(VideoDriverMode mode)
	{
		assert(videoDriver == nullptr);

		switch (mode)
		{
		case VIDEO_DRIVER_MODE_NULL:
			videoDriver = new VideoDriver;
			break;
		case VIDEO_DRIVER_MODE_D3D11:
			videoDriver = new VideoDriverD3D11;
			break;
		default:
			LOG_WARN << "Undefined Video Driver Mode : " << mode;
			videoDriver = new VideoDriver;
			break;
		}
	}

	void VideoDriver::destroyVideoDriver()
	{
		if (videoDriver)
		{
			videoDriver->finalize();
			delete videoDriver;
			videoDriver = nullptr;
		}
	}
}