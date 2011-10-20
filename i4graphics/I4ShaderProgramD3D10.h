#pragma once

#include "i4graphics.h"
#include "I4VideoDriverD3D10.h"
#include "I4ShaderProgram.h"

namespace i4graphics
{
	class I4ShaderProgramD3D10 : public I4ShaderProgram
	{
	public:
		I4ShaderProgramD3D10(ID3D10Device* device);
		virtual ~I4ShaderProgramD3D10();

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements) override;

		virtual bool	begin() override;
		virtual void	end() override;
		virtual void	apply() override;

		virtual void	setBool(I4ShaderBool sb, bool v) override;
		virtual void	setInt(I4ShaderInt si, int v) override;
		virtual void	setFloat(I4ShaderFloat sf, float v) override;
		virtual void	setVector(I4ShaderVector sv, const float* v) override;
		virtual void	setVectorArray(I4ShaderVectorArray sva, const float* v, unsigned int offset, unsigned int count) override;
		virtual void	setMatrix(I4ShaderMatrix sm, const float* v) override;
		virtual void	setMatrixArray(I4ShaderMatrixArray sva, const float* v, unsigned int offset, unsigned int count) override;
		virtual void	setTexture(I4ShaderTexture st, const I4Texture* tex) override;
		virtual void	setRenderTarget(I4ShaderRenderTarget srt, const I4RenderTarget* rt) override;

	private:
		void	mapShaderBool(I4ShaderBool sb, const char* name);
		void	mapShaderInt(I4ShaderInt si, const char* name);
		void	mapShaderFloat(I4ShaderFloat sf, const char* name);
		void	mapShaderVector(I4ShaderVector sv, const char* name);
		void	mapShaderVectorArray(I4ShaderVectorArray sva, const char* name);
		void	mapShaderMatrix(I4ShaderMatrix sm, const char* name);
		void	mapShaderMatrixArray(I4ShaderMatrixArray sva, const char* name);
		void	mapShaderTexture(I4ShaderTexture st, const char* name);
		void	mapShaderRenderTarget(I4ShaderRenderTarget srt, const char* name);

	private:
		ID3D10Device*			d3dDevice;
		ID3D10Effect*			effect;		
		ID3D10EffectTechnique*	technique;
		ID3D10InputLayout*		vertexLayout;
		D3D10_EFFECT_DESC		effectDesc;
		D3D10_TECHNIQUE_DESC	techniqueDesc;

		ID3D10EffectScalarVariable*			shaderBool[I4SHADER_BOOL_TOTAL_NUM];
		ID3D10EffectScalarVariable*			shaderInt[I4SHADER_INT_TOTAL_NUM];
		ID3D10EffectScalarVariable*			shaderFloat[I4SHADER_FLOAT_TOTAL_NUM];
		ID3D10EffectVectorVariable* 		shaderVector[I4SHADER_VECTOR_TOTAL_NUM];
		ID3D10EffectVectorVariable* 		shaderVectorArray[I4SHADER_VECTOR_ARRAY_TOTAL_NUM];
		ID3D10EffectMatrixVariable* 		shaderMatrix[I4SHADER_MATRIX_TOTAL_NUM];
		ID3D10EffectMatrixVariable* 		shaderMatrixArray[I4SHADER_MATRIX_ARRAY_TOTAL_NUM];
		ID3D10EffectShaderResourceVariable*	shaderTexture[I4SHADER_TEXTURE_TOTAL_NUM];
		ID3D10EffectShaderResourceVariable*	shaderRenderTarget[I4SHADER_RENDER_TARGET_TOTAL_NUM];
	};

}