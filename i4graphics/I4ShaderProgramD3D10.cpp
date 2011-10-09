#include "I4ShaderProgramD3D10.h"
#include "I4Log.h"
#include "I4Vector3.h"
#include "I4Vector4.h"
#include "I4TextureD3D10.h"
#include "I4RenderTargetD3D10.h"

namespace i4graphics
{
	I4ShaderProgramD3D10::I4ShaderProgramD3D10(ID3D10Device* device)
		: d3dDevice(device)
		, effect(NULL)
	{
	}

	I4ShaderProgramD3D10::~I4ShaderProgramD3D10()
	{
		if (effect)
		{
			effect->Release();
			effect = NULL;
		}
	}

	bool I4ShaderProgramD3D10::createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		HRESULT hr = S_OK;

		ID3D10Blob* bufferErrors = NULL;
		DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
		dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

		hr = D3DX10CreateEffectFromMemory(code, strlen(code), NULL, NULL, NULL, "fx_4_0", dwShaderFlags, 0, d3dDevice, NULL, NULL, &effect, &bufferErrors, NULL);
		if (FAILED(hr))
		{			
			if (bufferErrors)
			{
				I4LOG_WARN << (const char*)bufferErrors->GetBufferPointer();

				bufferErrors->Release();
			}

			return false;
		}

		if (bufferErrors)
		{
			bufferErrors->Release();
		}

		effect->GetDesc(&effectDesc);
		
		technique = effect->GetTechniqueByIndex(0);
		technique->GetDesc(&techniqueDesc);
		
		D3D10_PASS_DESC PassDesc;
		technique->GetPassByIndex(0)->GetDesc(&PassDesc);
		hr = d3dDevice->CreateInputLayout((const D3D10_INPUT_ELEMENT_DESC*)inputElements, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &vertexLayout);
		if (FAILED(hr))
		{
			I4LOG_WARN << L"create input layout failed.";
			return false;
		}

		for (int i = 0; i < I4SHADER_BOOL_TOTAL_NUM; ++i)
		{
			mapShaderBool((I4ShaderBool)i, I4SHADER_BOOL_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_INT_TOTAL_NUM; ++i)
		{
			mapShaderInt((I4ShaderInt)i, I4SHADER_INT_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_FLOAT_TOTAL_NUM; ++i)
		{
			mapShaderFloat((I4ShaderFloat)i, I4SHADER_FLOAT_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_VECTOR_TOTAL_NUM; ++i)
		{
			mapShaderVector((I4ShaderVector)i, I4SHADER_VECTOR_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_VECTOR_ARRAY_TOTAL_NUM; ++i)
		{
			mapShaderVectorArray((I4ShaderVectorArray)i, I4SHADER_VECTOR_ARRAY_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_MATRIX_TOTAL_NUM; ++i)
		{
			mapShaderMatrix((I4ShaderMatrix)i, I4SHADER_MATRIX_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_MATRIX_ARRAY_TOTAL_NUM; ++i)
		{
			mapShaderMatrixArray((I4ShaderMatrixArray)i, I4SHADER_MATRIX_ARRAY_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_TEXTURE_TOTAL_NUM; ++i)
		{
			mapShaderTexture((I4ShaderTexture)i, I4SHADER_TEXTURE_NAME[i]);
		}

		for (int i = 0; i < I4SHADER_RENDER_TARGET_TOTAL_NUM; ++i)
		{
			mapShaderRenderTarget((I4ShaderRenderTarget)i, I4SHADER_RENDER_TARGET_NAME[i]);
		}

		effect->Optimize();

		return true;
	}

	bool I4ShaderProgramD3D10::begin()
	{
		d3dDevice->IASetInputLayout(vertexLayout);

		return true;
	}

	void I4ShaderProgramD3D10::end()
	{
	}

	void I4ShaderProgramD3D10::apply()
	{		
		technique->GetPassByIndex(0)->Apply(0);
	}

	void I4ShaderProgramD3D10::setBool(I4ShaderBool sb, bool v)
	{
		if (shaderBool[sb] != NULL)
		{
			shaderBool[sb]->SetBool(v);
		}
	}

	void I4ShaderProgramD3D10::setInt(I4ShaderInt si, int v)
	{
		if (shaderInt[si] != NULL)
		{
			shaderInt[si]->SetInt(v);
		}
	}

	void I4ShaderProgramD3D10::setFloat(I4ShaderFloat sf, float v)
	{
		if (shaderFloat[sf] != NULL)
		{
			shaderFloat[sf]->SetFloat(v);
		}
	}

	void I4ShaderProgramD3D10::setVector(I4ShaderVector sv, float* v)
	{
		if (shaderVector[sv] != NULL)
		{
			shaderVector[sv]->SetFloatVector(v);
		}
	}

	void I4ShaderProgramD3D10::setVectorArray(I4ShaderVectorArray sva, float* v, unsigned int offset, unsigned int count)
	{
		if (shaderVectorArray[sva] != NULL)
		{
			shaderVectorArray[sva]->SetFloatVectorArray(v, offset, count);
		}
	}
	
	void I4ShaderProgramD3D10::setMatrix(I4ShaderMatrix sm, float* v)
	{
		if (shaderMatrix[sm] != NULL)
		{
			shaderMatrix[sm]->SetMatrix(v);
		}
	}

	void I4ShaderProgramD3D10::setMatrixArray(I4ShaderMatrixArray sma, float* m, unsigned int offset, unsigned int count)
	{
		if (shaderMatrixArray[sma] != NULL)
		{
			shaderMatrixArray[sma]->SetMatrixArray(m, offset, count);
		}
	}
	
	void I4ShaderProgramD3D10::setTexture(unsigned int stage, I4Texture* tex)
	{
		if (tex != NULL)
		{
			ID3D10ShaderResourceView* shaderResourceView = static_cast<I4TextureD3D10*>(tex)->getShaderResourceView();
			d3dDevice->PSSetShaderResources(stage, 1, &shaderResourceView);
		}
		else
		{
			ID3D10ShaderResourceView* shaderResourceView = NULL;
			d3dDevice->PSSetShaderResources(stage, 1, &shaderResourceView);
		}
	}	
	
	void I4ShaderProgramD3D10::setRenderTarget(unsigned int stage, I4RenderTarget* rt)
	{
		if (rt != NULL)
		{
			ID3D10ShaderResourceView* shaderResourceView = static_cast<I4RenderTargetD3D10*>(rt)->getShaderResourceView();
			d3dDevice->PSSetShaderResources(stage, 1, &shaderResourceView);
		}
		else
		{
			ID3D10ShaderResourceView* shaderResourceView = NULL;
			d3dDevice->PSSetShaderResources(stage, 1, &shaderResourceView);
		}
	}	

	void I4ShaderProgramD3D10::setTexture(I4ShaderTexture st, I4Texture* tex)
	{
		if (shaderTexture[st] != NULL)
		{
			if (tex != NULL)
			{
				shaderTexture[st]->SetResource(static_cast<I4TextureD3D10*>(tex)->getShaderResourceView());
			}
			else
			{
				shaderTexture[st]->SetResource(NULL);
			}
		}
	}

	void I4ShaderProgramD3D10::setRenderTarget(I4ShaderRenderTarget srt, I4RenderTarget* rt)
	{
		if (shaderRenderTarget[srt] != NULL)
		{
			if (rt != NULL)
			{
				shaderRenderTarget[srt]->SetResource(static_cast<I4RenderTargetD3D10*>(rt)->getShaderResourceView());
			}
			else
			{
				shaderRenderTarget[srt]->SetResource(NULL);
			}
		}
	}

	void I4ShaderProgramD3D10::mapShaderBool(I4ShaderBool sb, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderBool[sb] = var->AsScalar();
		}
		else
		{
			shaderBool[sb] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderInt(I4ShaderInt si, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderInt[si] = var->AsScalar();
		}
		else
		{
			shaderInt[si] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderFloat(I4ShaderFloat sf, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderFloat[sf] = var->AsScalar();
		}
		else
		{
			shaderFloat[sf] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderVector(I4ShaderVector sv, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderVector[sv] = var->AsVector();
		}
		else
		{
			shaderVector[sv] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderVectorArray(I4ShaderVectorArray sva, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderVectorArray[sva] = var->AsVector();
		}
		else
		{
			shaderVectorArray[sva] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderMatrix(I4ShaderMatrix sm, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderMatrix[sm] = var->AsMatrix();
		}
		else
		{
			shaderMatrix[sm] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderMatrixArray(I4ShaderMatrixArray sva, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderMatrixArray[sva] = var->AsMatrix();
		}
		else
		{
			shaderMatrixArray[sva] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderTexture(I4ShaderTexture st, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderTexture[st] = var->AsShaderResource();
		}
		else
		{
			shaderTexture[st] = NULL;
		}
	}

	void I4ShaderProgramD3D10::mapShaderRenderTarget(I4ShaderRenderTarget srt, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderRenderTarget[srt] = var->AsShaderResource();
		}
		else
		{
			shaderRenderTarget[srt] = NULL;
		}
	}

}
