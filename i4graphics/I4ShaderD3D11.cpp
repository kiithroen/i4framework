#include "stdafx.h"
#include "I4ShaderD3D11.h"
#include "I4Log.h"
#include "I4Vector3.h"
#include "I4Vector4.h"
#include "I4TextureD3D11.h"
#include "I4RenderTargetD3D11.h"
#include "I4GeometryBufferD3D11.h"
#include <d3dcompiler.h>

namespace i4graphics
{
	
	

	ShaderD3D11::ShaderD3D11(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11SamplerState** states)
		: d3dDevice(device)
		, immediateContext(context)
		, samplerStates(states)
		, vertexShader(nullptr)
		, pixelShader(nullptr)
		, vertexLayout(nullptr)
	{
	}

	ShaderD3D11::~ShaderD3D11()
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

	bool ShaderD3D11::createFromString(const char* code, const INPUT_ELEMENT* inputElements, unsigned int numElements)
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

	bool ShaderD3D11::begin()
	{
		immediateContext->IASetInputLayout(vertexLayout);
		immediateContext->VSSetShader(vertexShader, nullptr, 0);
		immediateContext->PSSetShader(pixelShader, nullptr, 0);

		return true;
	}

	void ShaderD3D11::end()
	{
	}

	void ShaderD3D11::setConstantBuffer(ShaderType type, unsigned int slot, ConstantBuffer* constantBuffer, void* data)
	{
		ID3D11Buffer* buffer = ((ConstantBufferD3D11*)constantBuffer)->getBuffer();
		immediateContext->UpdateSubresource(buffer, 0, nullptr, data, 0, 0);

		if (type == SHADER_TYPE_VS)
		{
			immediateContext->VSSetConstantBuffers(slot, 1, &buffer);
		}
		else if (type == SHADER_TYPE_PS)
		{
			immediateContext->PSSetConstantBuffers(slot, 1, &buffer);
		}
		else if (type == SHADER_TYPE_GS)
		{
			immediateContext->GSSetConstantBuffers(slot, 1, &buffer);
		}
	}

	void ShaderD3D11::setTexture(unsigned int slot, const Texture* tex)
	{
		if (tex != nullptr)
		{
			ID3D11ShaderResourceView* texRV = ((TextureD3D11*)tex)->getShaderResourceView();
			immediateContext->PSSetShaderResources(slot, 1, &texRV);
		}
		else
		{
			ID3D11ShaderResourceView * const nullRes[1] = {nullptr};
			immediateContext->PSSetShaderResources(slot, 1, nullRes);
		}
	}

	void ShaderD3D11::setRenderTarget(unsigned int slot, const RenderTarget* tex)
	{
		if (tex != nullptr)
		{
			ID3D11ShaderResourceView* texRV = ((RenderTargetD3D11*)tex)->getShaderResourceView();
			immediateContext->PSSetShaderResources(slot, 1, &texRV);
		}
		else
		{
			ID3D11ShaderResourceView * const nullRes[1] = {nullptr};
			immediateContext->PSSetShaderResources(slot, 1, nullRes);
		}
	}

	void ShaderD3D11::setSamplerState(unsigned int slot, SamplerState state)
	{
		immediateContext->PSSetSamplers(slot, 1, &samplerStates[state]);
	}

	bool ShaderD3D11::compileShaderFromString(const char* code, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut)
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
			LOG_WARN << (char*)pErrorBlob->GetBufferPointer();
			pErrorBlob->Release();
		}

		if (FAILED(hr))
		{
			return false;
		}
		
		return true;
	}
}
