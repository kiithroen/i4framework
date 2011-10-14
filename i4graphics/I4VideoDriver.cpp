#include "I4VideoDriver.h"
#include "I4VideoDriverD3D10.h"
#include "I4ShaderProgram.h"
#include "I4GeometryBuffer.h"
#include "I4Texture.h"
#include "I4RenderTarget.h"
#include "I4MathUtil.h"
#include "I4Log.h"

namespace i4graphics
{
	I4VideoDriver::I4VideoDriver()
		: width(0)
		, height(0)
		, activeShaderProgram(NULL)
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

		return true;
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

	void I4VideoDriver::clearScreen(unsigned char r, unsigned char g, unsigned char b)
	{
	}

	void I4VideoDriver::clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a)
	{
	}

	void I4VideoDriver::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
	}

	void I4VideoDriver::setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget, bool isDepthStencil)
	{
	}

	void I4VideoDriver::resetRenderTarget()
	{
	}

	I4ShaderProgram* I4VideoDriver::createShaderProgram()
	{
		return new I4ShaderProgram;
	}

	I4VertexBuffer* I4VideoDriver::createVertexBuffer()
	{
		return new I4VertexBuffer;
	}

	I4IndexBuffer* I4VideoDriver::createIndexBuffer()
	{
		return new I4IndexBuffer;
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

	I4VideoDriver* I4VideoDriver::videoDriver = NULL;

	void I4VideoDriver::createVideoDriver(I4VideoDriverMode mode)
	{
		assert(videoDriver == NULL);

		switch (mode)
		{
		case I4VIDEO_DRIVER_MODE_NULL:
			videoDriver = new I4VideoDriver;
			break;
		case I4VIDEO_DRIVER_MODE_D3D10:
			videoDriver = new I4VideoDriverD3D10;
			break;
		default:
			I4LOG_WARN << "Undefined Video Driver Mode : " << mode;
			videoDriver = new I4VideoDriver;
			break;
		}
	}

	void I4VideoDriver::destroyVideoDriver()
	{
		delete videoDriver;
		videoDriver = NULL;
	}
}