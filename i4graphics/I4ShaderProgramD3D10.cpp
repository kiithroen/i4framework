#include "I4ShaderProgramD3D10.h"
#include "I4TextureD3D10.h"
#include "I4Log.h"
#include "I4Vector3.h"
#include "I4Vector4.h"

namespace i4graphics {

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

		for (int i = 0; i < SHADER_VECTOR_TOTAL_NUM; ++i)
		{
			mapShaderVector((ShaderVector)i, SHADER_VECTOR_NAME[i]);
		}

		for (int i = 0; i < SHADER_VECTOR_ARRAY_TOTAL_NUM; ++i)
		{
			mapShaderVectorArray((ShaderVectorArray)i, SHADER_VECTOR_ARRAY_NAME[i]);
		}

		for (int i = 0; i < SHADER_MATRIX_TOTAL_NUM; ++i)
		{
			mapShaderMatrix((ShaderMatrix)i, SHADER_MATRIX_NAME[i]);
		}

		for (int i = 0; i < SHADER_MATRIX_ARRAY_TOTAL_NUM; ++i)
		{
			mapShaderMatrixArray((ShaderMatrixArray)i, SHADER_MATRIX_ARRAY_NAME[i]);
		}

		for (int i = 0; i < SHADER_TEXTURE_TOTAL_NUM; ++i)
		{
			mapShaderResource((ShaderTexture)i, SHADER_TEXTURE_NAME[i]);
		}

		return true;
	}

	bool I4ShaderProgramD3D10::begin()
	{
		d3dDevice->IASetInputLayout(vertexLayout);
		technique->GetPassByIndex(0)->Apply(0);

		return true;

	}

	void I4ShaderProgramD3D10::end()
	{
	}

	void I4ShaderProgramD3D10::setVector(ShaderVector sv, float* v)
	{
		if (shaderVector[sv] != NULL)
		{
			shaderVector[sv]->SetFloatVector(v);
		}
	}


	void I4ShaderProgramD3D10::setVectorArray(ShaderVectorArray sva, float* v, unsigned int offset, unsigned int count)
	{
		if (shaderVectorArray[sva] != NULL)
		{
			shaderVectorArray[sva]->SetFloatVectorArray(v, offset, count);
		}
	}
	
	void I4ShaderProgramD3D10::setMatrix(ShaderMatrix sm, float* v)
	{
		if (shaderMatrix[sm] != NULL)
		{
			shaderMatrix[sm]->SetMatrix(v);
		}
	}

	void I4ShaderProgramD3D10::setMatrixArray(ShaderMatrixArray sma, float* m, unsigned int offset, unsigned int count)
	{
		if (shaderMatrixArray[sma] != NULL)
		{
			shaderMatrixArray[sma]->SetMatrixArray(m, offset, count);
		}
	}
	
	void I4ShaderProgramD3D10::setTexture(ShaderTexture st, I4Texture* tex)
	{
		if (shaderResource[st] != NULL)
		{
			if (tex != NULL)
			{
				shaderResource[st]->SetResource(static_cast<I4TextureD3D10*>(tex)->get());
			}
			else
			{
				shaderResource[st]->SetResource(NULL);
			}
		}
	}	
	
	void I4ShaderProgramD3D10::mapShaderVector(ShaderVector sv, const char* name)
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

	void I4ShaderProgramD3D10::mapShaderVectorArray(ShaderVectorArray sva, const char* name)
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

	void I4ShaderProgramD3D10::mapShaderMatrix(ShaderMatrix sm, const char* name)
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

	void I4ShaderProgramD3D10::mapShaderMatrixArray(ShaderMatrixArray sva, const char* name)
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

	void I4ShaderProgramD3D10::mapShaderResource(ShaderTexture st, const char* name)
	{
		ID3D10EffectVariable* var = effect->GetVariableBySemantic(name);
		if (var->IsValid())
		{
			shaderResource[st] = var->AsShaderResource();
		}
		else
		{
			shaderResource[st] = NULL;
		}
	}


}
