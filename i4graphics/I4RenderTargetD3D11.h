#pragma once

#include "i4graphics.h"
#include "I4VideoDriverD3D11.h"
#include "I4RenderTarget.h"

namespace i4graphics
{
	class I4RenderTargetD3D11 : public I4RenderTarget
	{
		friend class I4VideoDriverD3D11;
	public:
		virtual ~I4RenderTargetD3D11();

		virtual bool				create(unsigned int width, unsigned int height, I4FORMAT format) override;
		virtual bool				createDepthStencil(unsigned int width, unsigned int height, I4FORMAT texFormat, I4FORMAT srvFormat, I4FORMAT dsvFormat)	override;

		ID3D11RenderTargetView*		getRenderTargetView() const	{ return renderTargetView; }
		ID3D11DepthStencilView*		getDepthStencilView() const	{ return depthStencilView; }
		ID3D11ShaderResourceView*	getShaderResourceView() const { return shaderResourceView; }

	private:
		void						unload();

	private:
		I4RenderTargetD3D11(ID3D11Device* d3dDevice);

	private:
		ID3D11Device*				d3dDevice;
		ID3D11Texture2D*			renderTargetTex;
		ID3D11RenderTargetView*		renderTargetView;
		ID3D11DepthStencilView*		depthStencilView;
		ID3D11ShaderResourceView*	shaderResourceView;
	};

}