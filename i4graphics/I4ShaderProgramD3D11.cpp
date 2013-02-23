#include "stdafx.h"
#include "I4ShaderProgramD3D11.h"
#include "I4Log.h"
#include "I4Vector3.h"
#include "I4Vector4.h"
#include "I4TextureD3D11.h"
#include "I4RenderTargetD3D11.h"
#include "I4GeometryBufferD3D11.h"
#include <d3dcompiler.h>

namespace i4graphics
{
	
	

	I4ShaderProgramD3D11::I4ShaderProgramD3D11(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11SamplerState** states)
		: d3dDevice(device)
		, immediateContext(context)
		, samplerStates(states)
		, vertexShader(nullptr)
		, pixelShader(nullptr)
		, vertexLayout(nullptr)
	{
	}

	I4ShaderProgramD3D11::~I4ShaderProgramD3D11()
	{
		if (vertexLayout)
		{
			vertexLayout->Release();
			vertexLayout = nullptr;
		}

		if (pixelShader)
		{
			pixelShader->Release();
			pixelShader = nullptr;
		}

		if (vertexShader)
		{
			vertexShader->Release();
			vertexShader = nullptr;
		}
	}

	bool I4ShaderProgramD3D11::createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		HRESULT hr = S_OK;

		// Compile the vertex shader
		ID3DBlob* pVSBlob = nullptr;
		if (compileShaderFromString(code, "VS", "vs_4_0", &pVSBlob) == false)
			return false;

		// Create the vertex shader
		hr = d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);
		if (FAILED(hr))
		{	
			pVSBlob->Release();
			return false;
		}
		
		 // Create the input layout
		hr = d3dDevice->CreateInputLayout((const D3D11_INPUT_ELEMENT_DESC*)inputElements, numElements, pVSBlob->GetBufferPointer(),
											  pVSBlob->GetBufferSize(), &vertexLayout);
		pVSBlob->Release();
		if (FAILED(hr))
			return false;

		// Compile the pixel shader
		ID3DBlob* pPSBlob = nullptr;
		if (compileShaderFromString(code, "PS", "ps_4_0", &pPSBlob))
		{
			// Create the pixel shader
			hr = d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader);
			pPSBlob->Release();
			if (FAILED(hr))
				return false;
		}
		 
		return true;
	}

	bool I4ShaderProgramD3D11::begin()
	{
		immediateContext->IASetInputLayout(vertexLayout);
		immediateContext->VSSetShader(vertexShader, nullptr, 0);
		immediateContext->PSSetShader(pixelShader, nullptr, 0);

		return true;
	}

	void I4ShaderProgramD3D11::end()
	{
	}

	void I4ShaderProgramD3D11::setConstantBuffer(I4ShaderProgramType type, unsigned int slot, I4ConstantBuffer* constantBuffer, void* data)
	{
		ID3D11Buffer* buffer = ((I4ConstantBufferD3D11*)constantBuffer)->getBuffer();
		immediateContext->UpdateSubresource(buffer, 0, nullptr, data, 0, 0);

		if (type == I4SHADER_PROGRAM_TYPE_VS)
		{
			immediateContext->VSSetConstantBuffers(slot, 1, &buffer);
		}
		else if (type == I4SHADER_PROGRAM_TYPE_PS)
		{
			immediateContext->PSSetConstantBuffers(slot, 1, &buffer);
		}
		else if (type == I4SHADER_PROGRAM_TYPE_GS)
		{
			immediateContext->GSSetConstantBuffers(slot, 1, &buffer);
		}
	}

	void I4ShaderProgramD3D11::setTexture(unsigned int slot, const I4Texture* tex)
	{
		if (tex != nullptr)
		{
			ID3D11ShaderResourceView* texRV = ((I4TextureD3D11*)tex)->getShaderResourceView();
			immediateContext->PSSetShaderResources(slot, 1, &texRV);
		}
		else
		{
			ID3D11ShaderResourceView * const nullRes[1] = {nullptr};
			immediateContext->PSSetShaderResources(slot, 1, nullRes);
		}
	}

	void I4ShaderProgramD3D11::setRenderTarget(unsigned int slot, const I4RenderTarget* tex)
	{
		if (tex != nullptr)
		{
			ID3D11ShaderResourceView* texRV = ((I4RenderTargetD3D11*)tex)->getShaderResourceView();
			immediateContext->PSSetShaderResources(slot, 1, &texRV);
		}
		else
		{
			ID3D11ShaderResourceView * const nullRes[1] = {nullptr};
			immediateContext->PSSetShaderResources(slot, 1, nullRes);
		}
	}

	void I4ShaderProgramD3D11::setSamplerState(unsigned int slot, I4SamplerState state)
	{
		immediateContext->PSSetSamplers(slot, 1, &samplerStates[state]);
	}

	bool I4ShaderProgramD3D11::compileShaderFromString(const char* code, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS|D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
	#if defined( DEBUG ) || defined( _DEBUG )
		dwShaderFlags |= D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

		ID3DBlob* pErrorBlob = nullptr;
		hr = D3DCompile(code, strlen(code), nullptr, nullptr, nullptr, entryPoint, shaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

		if (pErrorBlob)
		{
			I4LOG_WARN << (char*)pErrorBlob->GetBufferPointer();
			pErrorBlob->Release();
		}

		if (FAILED(hr))
		{
			return false;
		}
		
		return true;
	}
}
