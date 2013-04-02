#pragma once

#include "I4GeometryBuffer.h"

namespace i4graphics
{
	enum ShaderType
	{
		SHADER_TYPE_VS = 0,
		SHADER_TYPE_PS,
		SHADER_TYPE_GS,

	};

	class Texture;
	class RenderTarget;
	enum SamplerState;

	class Shader
	{
	public:
		Shader(void)			{}
		virtual ~Shader(void)	{}

		virtual bool	createFromString(const char* code, const INPUT_ELEMENT* inputElements, unsigned int numElements)	{ return true; }

		virtual bool	begin()	{ return true; }
		virtual void	end()	{}
		
		virtual void	setConstantBuffer(ShaderType type, unsigned int slot, ConstantBuffer* constantBuffer, void* data)		{}
		virtual void	setTexture(unsigned int slot, const Texture* tex)				{}
		virtual void	setRenderTarget(unsigned int slot, const RenderTarget* tex)	{}
		virtual void	setSamplerState(unsigned int slot, SamplerState state)		{}

	};
}