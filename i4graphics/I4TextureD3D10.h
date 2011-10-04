#pragma once

#include "i4graphics.h"
#include "I4VideoDriverD3D10.h"
#include "I4Texture.h"

namespace i4graphics
{
	class I4TextureD3D10 : public I4Texture
	{
		friend class I4VideoDriverD3D10;
	public:
		virtual ~I4TextureD3D10();

		virtual bool				load(const char* fname) override;
		
		ID3D10ShaderResourceView*	get() const	{ return shaderResourceView; }

	private:
		void						unload();

	private:
		I4TextureD3D10(ID3D10Device* d3dDevice);

	private:
		ID3D10Device*				d3dDevice;
		ID3D10ShaderResourceView*	shaderResourceView;	
	};

}