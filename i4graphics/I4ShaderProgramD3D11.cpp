#include "I4ShaderProgramD3D11.h"
#include "I4Log.h"
#include "I4Vector3.h"
#include "I4Vector4.h"
#include "I4TextureD3D11.h"
#include "I4RenderTargetD3D11.h"
#include <d3dcompiler.h>

namespace i4graphics
{
	
	typedef std::map<std::string, ID3D11Buffer*>	ConstantBufferMap;
	ConstantBufferMap		constantBufferMap;
	ID3D11SamplerState*		sampler[I4SAMPLER_STATE_NUM];

	I4ShaderProgramD3D11::I4ShaderProgramD3D11(ID3D11Device* device, ID3D11DeviceContext* context)
		: d3dDevice(device)
		, immediateContext(context)
		, vertexShader(NULL)
		, pixelShader(NULL)
		, vertexLayout(NULL)
	{
	}

	I4ShaderProgramD3D11::~I4ShaderProgramD3D11()
	{
		if (vertexLayout)
		{
			vertexLayout->Release();
			vertexLayout = NULL;
		}

		if (pixelShader)
		{
			pixelShader->Release();
			pixelShader = NULL;
		}

		if (vertexShader)
		{
			vertexShader->Release();
			vertexShader = NULL;
		}
	}

	bool I4ShaderProgramD3D11::createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements)
	{
		HRESULT hr = S_OK;

		// Compile the vertex shader
		ID3DBlob* pVSBlob = NULL;
		if (compileShaderFromString(code, "VS", "vs_4_0", &pVSBlob) == false)
			return false;

		// Create the vertex shader
		hr = d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &vertexShader);
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
		ID3DBlob* pPSBlob = NULL;
		hr = compileShaderFromString(code, "PS", "ps_4_0", &pPSBlob);
		if (FAILED(hr))
			return false;

		// Create the pixel shader
		hr = d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pixelShader);
		pPSBlob->Release();
		if (FAILED(hr))
			return false;

		return true;
	}

	bool I4ShaderProgramD3D11::begin()
	{
		immediateContext->IASetInputLayout(vertexLayout);
		immediateContext->VSSetShader(vertexShader, NULL, 0);
		immediateContext->PSSetShader(pixelShader, NULL, 0);

		return true;
	}

	void I4ShaderProgramD3D11::end()
	{
	}

	void I4ShaderProgramD3D11::setConstantBuffer(I4ShaderProgramType type, unsigned int slot, const char* name, unsigned int size, void* buffer)
	{
		auto itr = constantBufferMap.find(name);
		
		ID3D11Buffer* constantBuffer = NULL;

		if (itr == constantBufferMap.end())
		{			
			D3D11_BUFFER_DESC bd;
			ZeroMemory( &bd, sizeof(bd) );
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = size;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;
			HRESULT hr = d3dDevice->CreateBuffer(&bd, NULL, &constantBuffer);
			if (FAILED(hr))
			{
				I4LOG_WARN << L"can't create constant buffer : " << name;
				return;
			}

			constantBufferMap.insert(std::make_pair(name, constantBuffer));
		}
		else
		{
			constantBuffer = itr->second;
		}

		immediateContext->UpdateSubresource(constantBuffer, 0, NULL, buffer, 0, 0);

		if (type == I4SHADER_PROGRAM_TYPE_VS)
		{
			immediateContext->VSSetConstantBuffers(slot, 1, &constantBuffer);
		}
		else if (type == I4SHADER_PROGRAM_TYPE_PS)
		{
			immediateContext->PSSetConstantBuffers(slot, 1, &constantBuffer);
		}
		else if (type == I4SHADER_PROGRAM_TYPE_GS)
		{
			immediateContext->GSSetConstantBuffers(slot, 1, &constantBuffer);
		}
	}

	void I4ShaderProgramD3D11::setTexture(unsigned int slot, const I4Texture* tex)
	{
		if (tex != NULL)
		{
			ID3D11ShaderResourceView* texRV = ((I4TextureD3D11*)tex)->getShaderResourceView();
			immediateContext->PSSetShaderResources(slot, 1, &texRV);
		}
		else
		{
			ID3D11ShaderResourceView * const nullRes[1] = {NULL};
			immediateContext->PSSetShaderResources(slot, 1, nullRes);
		}
	}

	void I4ShaderProgramD3D11::setRenderTarget(unsigned int slot, const I4RenderTarget* tex)
	{
		if (tex != NULL)
		{
			ID3D11ShaderResourceView* texRV = ((I4RenderTargetD3D11*)tex)->getShaderResourceView();
			immediateContext->PSSetShaderResources(slot, 1, &texRV);
		}
		else
		{
			ID3D11ShaderResourceView * const nullRes[1] = {NULL};
			immediateContext->PSSetShaderResources(slot, 1, nullRes);
		}
	}

	void I4ShaderProgramD3D11::setSamplerState(unsigned int slot, I4SamplerState state)
	{
		if (sampler[state] == NULL)
		{
			if (state == I4SAMPLER_STATE_POINT)
			{
				D3D11_SAMPLER_DESC sampDescPoint;
				ZeroMemory( &sampDescPoint, sizeof(sampDescPoint) );
				sampDescPoint.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				sampDescPoint.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDescPoint.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDescPoint.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDescPoint.ComparisonFunc = D3D11_COMPARISON_NEVER;
				sampDescPoint.MinLOD = 0;
				sampDescPoint.MaxLOD = D3D11_FLOAT32_MAX;
				HRESULT hr = d3dDevice->CreateSamplerState(&sampDescPoint, &sampler[I4SAMPLER_STATE_POINT]);
				if (FAILED(hr))
				{
					I4LOG_WARN << L"can't create sampler state point";
					return;
				}
			}
			else if (state == I4SAMPLER_STATE_LINEAR)
			{
				D3D11_SAMPLER_DESC sampDescLinear;
				ZeroMemory( &sampDescLinear, sizeof(sampDescLinear) );
				sampDescLinear.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				sampDescLinear.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDescLinear.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDescLinear.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				sampDescLinear.ComparisonFunc = D3D11_COMPARISON_NEVER;
				sampDescLinear.MinLOD = 0;
				sampDescLinear.MaxLOD = D3D11_FLOAT32_MAX;
				HRESULT hr = d3dDevice->CreateSamplerState(&sampDescLinear, &sampler[I4SAMPLER_STATE_LINEAR]);
				if (FAILED(hr))
				{
					I4LOG_WARN << L"can't create sampler state linear";
					return;
				}
			}
		}

		immediateContext->PSSetSamplers(slot, 1, &sampler[state]);
	}

	bool I4ShaderProgramD3D11::compileShaderFromString(const char* code, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS|D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
	#if defined( DEBUG ) || defined( _DEBUG )
		dwShaderFlags |= D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

		ID3DBlob* pErrorBlob;
		hr = D3DCompile(code, strlen(code), NULL, NULL, NULL, entryPoint, shaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
		if (FAILED(hr))
		{
			if (pErrorBlob != NULL)
			{
				I4LOG_WARN << (char*)pErrorBlob->GetBufferPointer();
				pErrorBlob->Release();
			}

			return false;
		}

		if (pErrorBlob)
		{
			pErrorBlob->Release();
		}

		return true;
	}
}
