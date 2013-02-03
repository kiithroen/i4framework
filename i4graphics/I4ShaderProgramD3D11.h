#pragma once

#include "i4graphics.h"
#include "I4VideoDriverD3D11.h"
#include "I4ShaderProgram.h"

namespace i4graphics
{
	class I4ShaderProgramD3D11 : public I4ShaderProgram
	{
		typedef std::map<std::string, ID3D11Buffer*>	ConstantBufferMap;
	public:
		I4ShaderProgramD3D11(ID3D11Device* device, ID3D11DeviceContext* context);
		virtual ~I4ShaderProgramD3D11();

		virtual bool	createFromString(const char* code, const I4INPUT_ELEMENT* inputElements, unsigned int numElements) override;

		virtual bool	begin() override;
		virtual void	end() override;

		virtual bool	mapConstantBuffer(const char* name, unsigned int size) override;

		virtual void	setConstantBuffer(I4ShaderProgramType type, unsigned int slot, const char* name, void* buffer) override;
		virtual void	setTexture(unsigned int slot, const I4Texture* tex)	override;

	private:
		bool compileShaderFromString(const char* code, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut);

	private:
		ID3D11Device*			d3dDevice;
		ID3D11DeviceContext*	immediateContext;
		ID3D11VertexShader*		vertexShader;
		ID3D11PixelShader*		pixelShader;
		ID3D11InputLayout*		vertexLayout;
		ConstantBufferMap		constantBufferMap;
	};

}