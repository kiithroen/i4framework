#pragma once

#include "I4Matrix4x4.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	class Texture;

	enum TexureProxyStatus
	{
		TEXTURE_PROXY_STATUS_NOT_LOADED = 0,
		TEXTURE_PROXY_STATUS_LOAD_FAILED,
		TEXTURE_PROXY_STATUS_OK,

	};

	struct TextureProxy
	{
		TextureProxy()
			: texture(nullptr)
			, loadCount(0)
			, status(TEXTURE_PROXY_STATUS_NOT_LOADED)
		{
		}

		Texture*				texture;
		int						loadCount;
		TexureProxyStatus		status;
		string					fname;
	};

	class TextureMgr
	{
		typedef std::map<Hash, TextureProxy>			TextureProxyMap;
	public:
		TextureMgr(void);
		~TextureMgr(void);

		Hash			load(const char* fname, bool loadNow = true);
		void			unload(Hash hash);

		Texture*		find(Hash hash);

	private:
		TextureProxyMap	mapTextureProxy;
	};

}
