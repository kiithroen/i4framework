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

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);

		virtual bool	begin();
		virtual void	end();

		virtual void	setVector(ShaderVector sv, float* v);
		virtual void	setVectorArray(ShaderVectorArray sva, float* v, unsigned int offset, unsigned int count);
		virtual void	setMatrix(ShaderMatrix sm, float* v);
		virtual void	setMatrixArray(ShaderMatrixArray sva, float* v, unsigned int offset, unsigned int count);
		virtual void	setTexture(ShaderTexture st, I4Texture* tex);

	private:
		void	mapShaderVector(ShaderVector sv, const char* name);
		void	mapShaderVectorArray(ShaderVectorArray sva, const char* name);
		void	mapShaderMatrix(ShaderMatrix sm, const char* name);
		void	mapShaderMatrixArray(ShaderMatrixArray sva, const char* name);		
		void	mapShaderResource(ShaderTexture st, const char* name);

	private:
		ID3D10Device*			d3dDevice;
		ID3D10Effect*			effect;		
		ID3D10EffectTechnique*	technique;
		ID3D10InputLayout*		vertexLayout;
		D3D10_EFFECT_DESC		effectDesc;
		D3D10_TECHNIQUE_DESC	techniqueDesc;

		ID3D10EffectVectorVariable* 		shaderVector[SHADER_VECTOR_TOTAL_NUM];
		ID3D10EffectVectorVariable* 		shaderVectorArray[SHADER_VECTOR_ARRAY_TOTAL_NUM];
		ID3D10EffectMatrixVariable* 		shaderMatrix[SHADER_MATRIX_TOTAL_NUM];
		ID3D10EffectMatrixVariable* 		shaderMatrixArray[SHADER_MATRIX_ARRAY_TOTAL_NUM];
		ID3D10EffectShaderResourceVariable*	shaderResource[SHADER_RESOURCE_TOTAL_NUM];
	};

}