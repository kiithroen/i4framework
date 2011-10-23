#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"

namespace i4graphics
{
	class I4ShaderProgram;
	class I4VertexBuffer;
	class I4IndexBuffer;
	class I4Texture;
	class I4RenderTarget;
	
	enum I4VideoDriverMode
	{
		I4VIDEO_DRIVER_MODE_NULL	= 0,
		I4VIDEO_DRIVER_MODE_D3D10,
	};

	class I4GRAPHICS_API I4VideoDriver
	{
	public:
		virtual ~I4VideoDriver();

		virtual I4VideoDriverMode	getVideoDriverMode() const	{ return I4VIDEO_DRIVER_MODE_NULL; }

		virtual bool				initialize(void* windowID, unsigned int width, unsigned int height);

		virtual bool				setupEnvironment();

		virtual bool				beginScene();
		virtual void				endScene();

		virtual void				clearBackBuffer(unsigned char r, unsigned char g, unsigned char b);
		virtual void				clearRenderTarget(I4RenderTarget* renderTarget, float r, float g, float b, float a);

		virtual void				setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		virtual void				setRenderTarget(unsigned int num, I4RenderTarget** arrRenderTarget, bool isDepthStencil);
		virtual void				resetRenderTarget();

		virtual I4ShaderProgram*	createShaderProgram();

		virtual I4VertexBuffer*		createVertexBuffer();
		virtual I4IndexBuffer*		createIndexBuffer();

		virtual I4Texture*			createTexture();

		virtual I4RenderTarget*		createRenderTarget();
		
		void						setActiveSahderProgram(I4ShaderProgram* shader)	{ activeShaderProgram = shader; }
		I4ShaderProgram*			getActiveSahderProgram() const					{ return activeShaderProgram; }

		unsigned int				getWidth() const		{ return width; }
		unsigned int				getHeight() const		{ return height; }
		
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
		I4ShaderProgram*			activeShaderProgram;

	private:
		static I4VideoDriver*		videoDriver;
	};

}