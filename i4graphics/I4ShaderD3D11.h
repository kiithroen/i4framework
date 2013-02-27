#pragma once

#include "i4graphics.h"
#include "I4VideoDriverD3D11.h"
#include "I4Shader.h"

namespace i4graphics
{
	class I4ShaderD3D11 : public I4Shader
	{
	public:
		I4ShaderD3D11(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11SamplerState** samplerStates);
		virtual ~I4ShaderD3D11();

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements) override;

		virtual bool	begin() override;
		virtual void	end() override;
		
		virtual void	setConstantBuffer(I4ShaderType type, unsigned int slot, I4ConstantBuffer* constantBuffer, void* data) override;
		virtual void	setTexture(unsigned int slot, const I4Texture* tex)	override;
		virtual void	setRenderTarget(unsigned int slot, const I4RenderTarget* tex) override;
		virtual void	setSamplerState(unsigned int slot, I4SamplerState state) override;

	private:
		bool compileShaderFromString(const char* code, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut);

	private:
		ID3D11Device*			d3dDevice;
		ID3D11DeviceContext*	immediateContext;
		ID3D11SamplerState**	samplerStates;
		ID3D11VertexShader*		vertexShader;
		ID3D11PixelShader*		pixelShader;
		ID3D11InputLayout*		vertexLayout;
	};

}