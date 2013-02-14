#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	class I4Texture;

	enum I4TexureProxyStatus
	{
		I4TEXTURE_PROXY_STATUS_NOT_LOADED = 0,
		I4TEXTURE_PROXY_STATUS_LOAD_FAILED,
		I4TEXTURE_PROXY_STATUS_OK,

	};

	struct I4TextureProxy
	{
		I4TextureProxy()
			: texture(nullptr)
			, loadCount(0)
			, status(I4TEXTURE_PROXY_STATUS_NOT_LOADED)
		{
		}

		I4Texture*				texture;
		int						loadCount;
		I4TexureProxyStatus		status;
		string					fname;
	};

	class I4TextureMgr
	{
		typedef std::map<I4Hash, I4TextureProxy>			I4TextureProxyMap;
	public:
		I4TextureMgr(void);
		~I4TextureMgr(void);

		I4Hash			load(const char* fname, bool loadNow = true);
		void			unload(I4Hash hash);

		I4Texture*		find(I4Hash hash);

	public:
		static I4TextureMgr& getTextureMgr()
		{
			static I4TextureMgr mgr;
			return mgr;
		}

	private:
		I4TextureProxyMap	mapTextureProxy;
	};

}
