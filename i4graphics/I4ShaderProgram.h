#pragma once

#include "i4graphics.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	enum I4ShaderProgramType
	{
		I4SHADER_PROGRAM_TYPE_VS = 0,
		I4SHADER_PROGRAM_TYPE_PS,
		I4SHADER_PROGRAM_TYPE_GS,

	};

	class I4Texture;
	class I4RenderTarget;
	enum I4SamplerState;

	class I4ShaderProgram
	{
	public:
		I4ShaderProgram(void)			{}
		virtual ~I4ShaderProgram(void)	{}

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)	{ return true; }

		virtual bool	begin()	{ return true; }
		virtual void	end()	{}
		
		virtual void	setConstantBuffer(I4ShaderProgramType type, unsigned int slot, const char* name, unsigned int size, void* buffer)		{}
		virtual void	setTexture(unsigned int slot, const I4Texture* tex)				{}
		virtual void	setRenderTarget(unsigned int slot, const I4RenderTarget* tex)	{}
		virtual void	setSamplerState(unsigned int slot, I4SamplerState state)		{}

	};
}