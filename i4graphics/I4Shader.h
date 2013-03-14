#pragma once

#include "I4GeometryBuffer.h"

namespace i4graphics
{
	enum I4ShaderType
	{
		I4SHADER_TYPE_VS = 0,
		I4SHADER_TYPE_PS,
		I4SHADER_TYPE_GS,

	};

	class I4Texture;
	class I4RenderTarget;
	enum I4SamplerState;

	class I4Shader
	{
	public:
		I4Shader(void)			{}
		virtual ~I4Shader(void)	{}

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)	{ return true; }

		virtual bool	begin()	{ return true; }
		virtual void	end()	{}
		
		virtual void	setConstantBuffer(I4ShaderType type, unsigned int slot, I4ConstantBuffer* constantBuffer, void* data)		{}
		virtual void	setTexture(unsigned int slot, const I4Texture* tex)				{}
		virtual void	setRenderTarget(unsigned int slot, const I4RenderTarget* tex)	{}
		virtual void	setSamplerState(unsigned int slot, I4SamplerState state)		{}

	};
}