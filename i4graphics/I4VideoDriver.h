#pragma once

#include "I4Matrix4x4.h"

namespace i4graphics
{
	class Shader;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class Texture;
	class RenderTarget;
	class TextureMgr;

	enum VideoDriverMode
	{
		VIDEO_DRIVER_MODE_NULL	= 0,
		VIDEO_DRIVER_MODE_D3D11,
	};

	enum RasterizerMode
	{		
		RASTERIZER_MODE_INVALID		= -1,
		RASTERIZER_MODE_SOLID_NONE	= 0,
		RASTERIZER_MODE_SOLID_FRONT,
		RASTERIZER_MODE_SOLID_BACK,
		RASTERIZER_MODE_WIRE_NONE,
		RASTERIZER_MODE_WIRE_FRONT,
		RASTERIZER_MODE_WIRE_BACK,
		RASTERIZER_MODE_NUM,
	};

	enum DepthStencilMode
	{
		DEPTHSTENCIL_MODE_INVAILD = -1,
		DEPTH_OFF_STENCIL_OFF = 0,
		DEPTH_LESS_STENCIL_OFF,
		DEPTH_GREATER_STENCIL_OFF,
		DEPTH_OFF_STENCIL_INC_ON_FAIL,
		DEPTH_LESS_STENCIL_INC_ON_FAIL,
		DEPTH_GREATER_STENCIL_INC_ON_FAIL,
		DEPTH_OFF_STENCIL_INC_ON_PASS,
		DEPTH_LESS_STENCIL_INC_ON_PASS,
		DEPTH_GREATER_STENCIL_INC_ON_PASS,
		DEPTHSTENCIL_MODE_NUM,
	};

	enum BlendMode
	{		
		BLEND_MODE_INVALID		= -1,
		BLEND_MODE_NONE			= 0,
		BLEND_MODE_ALPHA,
		BLEND_MODE_ADD,
		BLEND_MODE_NUM,
	};


	enum SamplerState
	{		
		SAMPLER_STATE_INVALID		= -1,
		SAMPLER_STATE_POINT		= 0,
		SAMPLER_STATE_LINEAR,
		SAMPLER_STATE_SHADOW,
		SAMPLER_STATE_NUM,
	};

	class VideoDriver
	{
	public:
		virtual ~VideoDriver();

		virtual VideoDriverMode	getVideoDriverMode() const	{ return VIDEO_DRIVER_MODE_NULL; }

		virtual bool				initialize(void* windowID, unsigned int width, unsigned int height);
		virtual void				finalize();

		virtual bool				setupEnvironment();

		virtual bool				beginScene();
		virtual void				endScene();

		virtual void				clearBackBuffer(unsigned char r, unsigned char g, unsigned char b);
		virtual void				clearRenderTarget(RenderTarget* renderTarget, float r, float g, float b, float a);		
		virtual void				clearDepthStencil(RenderTarget* renderTarget, float depth, unsigned char stencil);

		virtual void				setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		virtual void				resetViewport();

		virtual void				setRenderTarget(unsigned int num, RenderTarget** arrRenderTarget);
		virtual void				setRenderTarget(unsigned int num, RenderTarget** arrRenderTarget, RenderTarget* depthStencil);
		virtual void				setRenderTargetDepthStencil(RenderTarget* depthStencil);
		virtual void				resetBackBufferRenderTarget(bool enableDepthStencil);

		virtual void				setRasterizerMode(RasterizerMode mode);
		virtual void				setDepthStencilMode(DepthStencilMode mode);
		virtual void				setBlendMode(BlendMode mode);

		virtual Shader*	createShader();

		virtual VertexBuffer*		createVertexBuffer();
		virtual IndexBuffer*		createIndexBuffer();
		virtual ConstantBuffer*	createConstantBuffer();

		virtual Texture*			createTexture();

		virtual RenderTarget*		createRenderTarget();

		unsigned int				getWidth() const		{ return width; }
		unsigned int				getHeight() const		{ return height; }
		
		TextureMgr*				getTextureMgr()			{ return textureMgr; }
	public:
		static VideoDriver*		getVideoDriver()		{ return videoDriver; }

		static void					createVideoDriver(VideoDriverMode mode);
		static void					destroyVideoDriver();

	protected:		
		VideoDriver();

	protected:
		void*						windowID;
		unsigned int				width;
		unsigned int				height;		
		RasterizerMode			curRasterizerMode;
		DepthStencilMode			curDepthStencilMode;
		BlendMode					curBlendMode;
		TextureMgr*				textureMgr;

	private:
		static VideoDriver*		videoDriver;
	};

}