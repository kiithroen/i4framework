#pragma once

#include "I4VideoDriverD3D11.h"
#include "I4Texture.h"

namespace i4graphics
{
	class I4TextureD3D11 : public I4Texture
	{
		friend class I4VideoDriverD3D11;
	public:
		virtual ~I4TextureD3D11();

		virtual bool				load(const char* fname) override;
		
		ID3D11ShaderResourceView*	getShaderResourceView() const	{ return shaderResourceView; }

	private:
		void						unload();

	private:
		I4TextureD3D11(ID3D11Device* d3dDevice);

	private:
		ID3D11Device*				d3dDevice;
		ID3D11ShaderResourceView*	shaderResourceView;	
	};

}