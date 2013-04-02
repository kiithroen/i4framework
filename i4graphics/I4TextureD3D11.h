#pragma once

#include "I4VideoDriverD3D11.h"
#include "I4Texture.h"

namespace i4graphics
{
	class TextureD3D11 : public Texture
	{
		friend class VideoDriverD3D11;
	public:
		virtual ~TextureD3D11();

		virtual bool				load(const char* fname) override;
		
		ID3D11ShaderResourceView*	getShaderResourceView() const	{ return shaderResourceView; }

	private:
		void						unload();

	private:
		TextureD3D11(ID3D11Device* d3dDevice, ID3D11DeviceContext* _d3dContext);

	private:
		ID3D11Device*				d3dDevice;
		ID3D11DeviceContext*		d3dContext;
		ID3D11ShaderResourceView*	shaderResourceView;	
	};

}