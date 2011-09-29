#pragma once

#include "i4graphics.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	
	enum ShaderVector
	{
		SHADER_VECTOR_DUMMY = 0,
		SHADER_VECTOR_TOTAL_NUM,
	};

	static const char* SHADER_VECTOR_NAME[] = 
	{
		"DUMMY",
	};

	enum ShaderVectorArray
	{
		SHADER_VECTOR_ARRAY_LIGHTDIR = 0,
		SHADER_VECTOR_ARRAY_LIGHTCOLOR,
		SHADER_VECTOR_ARRAY_TOTAL_NUM,
	};

	static const char* SHADER_VECTOR_ARRAY_NAME[] = 
	{
		"LIGHTDIR",
		"LIGHTCOLOR",
	};

	enum ShaderMatrix
	{
		SHADER_MATRIX_WORLD = 0,
		SHADER_MATRIX_VIEW,
		SHADER_MATRIX_PROJECTION,
		SHADER_MATRIX_TOTAL_NUM,
	};

	static const char* SHADER_MATRIX_NAME[] = 
	{
		"WORLD",
		"VIEW",
		"PROJECTION",
	};

	enum ShaderMatrixArray
	{
		SHADER_MATRIX_ARRAY_PALETTE = 0,
		SHADER_MATRIX_ARRAY_TOTAL_NUM,
	};

	static const char* SHADER_MATRIX_ARRAY_NAME[] = 
	{
		"PALETTE",
	};

	enum ShaderTexture
	{
		SHADER_RESOURCE_DIFFUSEMAP = 0,
		SHADER_RESOURCE_TOTAL_NUM,
	};

	static const char* SHADER_RESOURCE_NAME[] = 
	{
		"DIFFUSEMAP",
	};

	class I4Texture;

	class I4ShaderProgram
	{
	public:
		I4ShaderProgram(void)			{}
		virtual ~I4ShaderProgram(void)	{}

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)	{ return true; }

		virtual bool	begin()	{ return true; }
		virtual void	end()	{}

		virtual void	setVector(ShaderVector sv, float* v)	{}
		virtual void	setVectorArray(ShaderVectorArray sva, float* v, unsigned int offset, unsigned int count) {}
		virtual void	setMatrix(ShaderMatrix sm, float* v) {}
		virtual void	setMatrixArray(ShaderMatrixArray sva, float* v, unsigned int offset, unsigned int count) {}
		virtual void	setTexture(ShaderTexture st, I4Texture* tex) {}
	};
}

