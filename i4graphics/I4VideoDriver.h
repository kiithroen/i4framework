#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"

namespace i4graphics
{
	class I4Shader;
	class I4VertexBuffer;
	class I4IndexBuffer;
	class I4ConstantBuffer;
	class I4Texture;
	class I4RenderTarget;
	class I4TextureMgr;

	enum I4VideoDriverMode
	{
		I4VIDEO_DRIVER_MODE_NULL	= 0,
		I4VIDEO_DRIVER_MODE_D3D11,
	};

	enum I4RasterizerMode
	{		
		I4RASTERIZER_MODE_INVALID		= -1,
		I4RASTERIZER_MODE_SOLID_NONE	= 0,
		I4RASTERIZER_MODE_SOLID_FRONT,
		I4RASTERIZER_MODE_SOLID_BACK,
		I4RASTERIZER_MODE_WIRE_NONE,
		I4RASTERIZER_MODE_WIRE_FRONT,
		I4RASTERIZER_MODE_WIRE_BACK,
		I4RASTERIZER_MODE_NUM,
	};

	enum I4BlendMode
	{		
		I4BLEND_MODE_INVALID		= -1,
		I4BLEND_MODE_NONE			= 0,
		I4BLEND_MODE_ALPHA,
		I4BLEND_MODE_ADD,
		I4BLEND_MODE_NUM,
	};


	enum I4SamplerState
	{		
		I4SAMPLER_STATE_INVALID		= -1,
		I4SAMPLER_STATE_POINT		= 0,
		I4SAMPLER_STATE_LINEAR,
		I4SAMPLER_STATE_SHADOW,
		I4SAMPLER_STATE_NUM,
	};

	class I4VideoDriver
	{
	public:
		virtual ~I4VideoDriver();

		virtual I4VideoDriverMode	getVideoDriverMode() const	{ return I4VIDEO_DRIVER_MODE_NULL; }

		virtual bool				initialize(void* windowID, unsigned int width, unsigned int height);
		virtual void				finalize();

		virtual bool				setupEnvironment();

		virtual bool				beginScene();
		virtual void				endScene();

		virtual void				clearBackBuffer(unsigned char r, unsigned char g, unsigned char b);
		virtual void				clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a);		
		virtual void				clearDepthStencil(I4RenderTarget* renderTarget, float depth, unsigned char stencil);

		virtual void				setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		virtual void				resetViewport();

		virtual void				setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget);
		virtual void				setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget, I4RenderTarget* depthStencil);
		virtual void				setRenderTargetDepthStencil(I4RenderTarget* depthStencil);
		virtual void				resetRenderTarget();

		virtual void				setRasterizerMode(I4RasterizerMode mode);
		virtual void				setBlendMode(I4BlendMode mode);

		virtual I4Shader*	createShader();

		virtual I4VertexBuffer*		createVertexBuffer();
		virtual I4IndexBuffer*		createIndexBuffer();
		virtual I4ConstantBuffer*	createConstantBuffer();

		virtual I4Texture*			createTexture();

		virtual I4RenderTarget*		createRenderTarget();

		unsigned int				getWidth() const		{ return width; }
		unsigned int				getHeight() const		{ return height; }
		
		I4TextureMgr*				getTextureMgr()			{ return textureMgr; }
	public:
		static I4VideoDriver*		getVideoDriver()		{ return videoDriver; }

		static void					createVideoDriver(I4VideoDriverMode mode);
		static void					destroyVideoDriver();

	protected:		
		I4VideoDriver();

	protected:
		void*						windowID;
		unsigned int				width;
		unsigned int				height;		
		I4RasterizerMode			curRasterizerMode;
		I4BlendMode					curBlendMode;
		I4TextureMgr*				textureMgr;

	private:
		static I4VideoDriver*		videoDriver;
	};

}