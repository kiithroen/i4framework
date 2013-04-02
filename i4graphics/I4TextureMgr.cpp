#include "stdafx.h"
#include "I4TextureMgr.h"
#include "I4VideoDriver.h"
#include "I4Texture.h"
#include "I4Log.h"

namespace i4graphics
{

	TextureMgr::TextureMgr(void)
	{
	}


	TextureMgr::~TextureMgr(void)
	{
		for (auto& itr : mapTextureProxy)
		{
			delete itr.second.texture;
		}

		mapTextureProxy.clear();
	}

	Hash TextureMgr::load(const char* fname, bool loadNow)
	{
		Hash hash(fname);

		auto itr = mapTextureProxy.find(hash);
		if (itr == mapTextureProxy.end())
		{
			Texture* texture = VideoDriver::getVideoDriver()->createTexture();

			TextureProxy proxy;
			proxy.fname = fname;
			proxy.loadCount = 1;
			proxy.texture = texture;

			if (loadNow)
			{
				if (texture->load(fname))
				{
					proxy.status = TEXTURE_PROXY_STATUS_OK;
				}
				else
				{
					proxy.status = TEXTURE_PROXY_STATUS_LOAD_FAILED;
				}
			}
			else
			{
				proxy.status = TEXTURE_PROXY_STATUS_NOT_LOADED;
			}

			mapTextureProxy.insert(make_pair(hash, proxy));
		}
		else
		{
			++itr->second.loadCount;
		}

		return hash;
	}
	
	void TextureMgr::unload(Hash hash)
	{
		if (hash == INVALID_HASHCODE)
			return;

		auto itr = mapTextureProxy.find(hash);
		if (itr == mapTextureProxy.end())
		{
			LOG_WARN << L"can't release texture proxy.";
			return;
		}

		--itr->second.loadCount;
		if (itr->second.loadCount == 0)
		{
			delete itr->second.texture;
			mapTextureProxy.erase(itr);
		}
	}

	Texture* TextureMgr::find(Hash hash)
	{
		if (hash == INVALID_HASHCODE)
			return nullptr;

		auto itr = mapTextureProxy.find(hash);
		if (itr == mapTextureProxy.end())
		{
			LOG_WARN << L"can't find texture proxy.";
			return nullptr;
		}
		
		if (itr->second.status == TEXTURE_PROXY_STATUS_OK)
		{
			return itr->second.texture;
		}
		else if (itr->second.status == TEXTURE_PROXY_STATUS_NOT_LOADED)
		{
			if (itr->second.texture->load(itr->second.fname.c_str()))
			{
				itr->second.status = TEXTURE_PROXY_STATUS_OK;
				return itr->second.texture;
			}
			else
			{
				itr->second.status = TEXTURE_PROXY_STATUS_LOAD_FAILED;
				return nullptr;
			}

		}
		else
		{
			return nullptr;
		}
	}
}