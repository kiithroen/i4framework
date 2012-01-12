#pragma once

#include "i4graphics.h"
#include "I4VideoDriverD3D10.h"
#include "I4RenderTarget.h"

namespace i4graphics
{
	class I4RenderTargetD3D10 : public I4RenderTarget
	{
		friend class I4VideoDriverD3D10;
	public:
		virtual ~I4RenderTargetD3D10();

		virtual bool				create(unsigned int width, unsigned int height, I4FORMAT format) override;
		virtual bool				createDepthStencil(unsigned int width, unsigned int height, I4FORMAT texFormat, I4FORMAT srvFormat, I4FORMAT dsvFormat)	override;

		ID3D10RenderTargetView*		getRenderTargetView() const	{ return renderTargetView; }
		ID3D10DepthStencilView*		getDepthStencilView() const	{ return depthStencilView; }
		ID3D10ShaderResourceView*	getShaderResourceView() const { return shaderResourceView; }

	private:
		void						unload();

	private:
		I4RenderTargetD3D10(ID3D10Device* d3dDevice);

	private:
		ID3D10Device*				d3dDevice;
		ID3D10Texture2D*			renderTargetTex;
		ID3D10RenderTargetView*		renderTargetView;
		ID3D10DepthStencilView*		depthStencilView;
		ID3D10ShaderResourceView*	shaderResourceView;
	};

}