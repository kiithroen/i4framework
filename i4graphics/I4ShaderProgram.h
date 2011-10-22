#pragma once

#include "i4graphics.h"
#include "I4GeometryBuffer.h"

namespace i4graphics
{
	enum I4ShaderBool
	{
		I4SHADER_BOOL_DUMMY = 0,
		I4SHADER_BOOL_TOTAL_NUM,
	};

	static const char* I4SHADER_BOOL_NAME[] = 
	{
		"DUMMY",
	};

	enum I4ShaderInt
	{
		I4SHADER_INT_DUMMY = 0,
		I4SHADER_INT_TOTAL_NUM,
	};

	static const char* I4SHADER_INT_NAME[] = 
	{
		"DUMMY",
	};

	enum I4ShaderFloat
	{		
		I4SHADER_FLOAT_SPECULAR_POWER = 0,
		I4SHADER_FLOAT_SPECULAR_INTENSITY,
		I4SHADER_FLOAT_FAR_DISTANCE,
		I4SHADER_FLOAT_TOTAL_NUM
	};

	static const char* I4SHADER_FLOAT_NAME[] = 
	{
		"SPECULAR_POWER",
		"SPECULAR_INTENSITY",
		"FAR_DISTANCE",
	};
	
	enum I4ShaderVector
	{
		I4SHADER_VECTOR_LIGHT_POINT_RADIUS = 0,
		I4SHADER_VECTOR_LIGHT_DIRECTION,
		I4SHADER_VECTOR_LIGHT_COLOR,
		I4SHADER_VECTOR_CAMERA_POSITION,
		I4SHADER_VECTOR_FAR_TOP_RIGHT,
		I4SHADER_VECTOR_TOTAL_NUM,
	};

	static const char* I4SHADER_VECTOR_NAME[] = 
	{
		"LIGHT_POINT_RADIUS",
		"LIGHT_DIRECTION",
		"LIGHT_COLOR",
		"CAMERA_POSITION",
		"FAR_TOP_RIGHT",
	};

	enum I4ShaderVectorArray
	{
		I4SHADER_VECTOR_ARRAY_DUMMY = 0,
		I4SHADER_VECTOR_ARRAY_TOTAL_NUM,
	};

	static const char* I4SHADER_VECTOR_ARRAY_NAME[] = 
	{
		"DUMMY",
	};

	enum I4ShaderMatrix
	{
		I4SHADER_MATRIX_WORLD = 0,
		I4SHADER_MATRIX_VIEW,
		I4SHADER_MATRIX_PROJECTION,
		I4SHADER_MATRIX_TOTAL_NUM,
	};

	static const char* I4SHADER_MATRIX_NAME[] = 
	{
		"WORLD",
		"VIEW",
		"PROJECTION",
	};

	enum I4ShaderMatrixArray
	{
		I4SHADER_MATRIX_ARRAY_PALETTE = 0,
		I4SHADER_MATRIX_ARRAY_TOTAL_NUM,
	};

	static const char* I4SHADER_MATRIX_ARRAY_NAME[] = 
	{
		"PALETTE",
	};
	
	enum I4ShaderTexture
	{
		I4SHADER_TEXTURE_DIFFUSEMAP = 0,
		I4SHADER_TEXTURE_NORMALMAP,
		I4SHADER_TEXTURE_SPECULARMAP,
		I4SHADER_TEXTURE_TOTAL_NUM,
	};

	static const char* I4SHADER_TEXTURE_NAME[] = 
	{
		"DIFFUSEMAP",
		"NORMALMAP",
		"SPECULARMAP",
	};

	enum I4ShaderRenderTarget
	{
		I4SHADER_RENDER_TARGET_DIFFUSE = 0,
		I4SHADER_RENDER_TARGET_NORMAL,
		I4SHADER_RENDER_TARGET_DEPTH,
		I4SHADER_RENDER_TARGET_LIGHT,
		I4SHADER_RENDER_TARGET_TOTAL_NUM,
	};

	static const char* I4SHADER_RENDER_TARGET_NAME[] = 
	{
		"RT_DIFFUSE",
		"RT_NORMAL",
		"RT_DEPTH",
		"RT_LIGHT",
	};

	class I4Texture;
	class I4RenderTarget;

	class I4ShaderProgram
	{
	public:
		I4ShaderProgram(void)			{}
		virtual ~I4ShaderProgram(void)	{}

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)	{ return true; }

		virtual bool	begin()	{ return true; }
		virtual void	end()	{}
		virtual void	apply()	{}

		virtual void	setBool(I4ShaderBool sb, bool v) {}
		virtual void	setInt(I4ShaderInt si, int v) {}
		virtual void	setFloat(I4ShaderFloat sf, float v) {}
		virtual void	setVector(I4ShaderVector sv, const float* v)	{}
		virtual void	setVectorArray(I4ShaderVectorArray sva, const float* v, unsigned int offset, unsigned int count) {}
		virtual void	setMatrix(I4ShaderMatrix sm, const float* v) {}
		virtual void	setMatrixArray(I4ShaderMatrixArray sva, const float* v, unsigned int offset, unsigned int count) {}
		virtual void	setTexture(I4ShaderTexture st, const I4Texture* tex) {}
		virtual void	setRenderTarget(I4ShaderRenderTarget srt, const I4RenderTarget* rt) {}

	};
}