#include "I4ActorMgr.h"
#include "I4Actor.h"
#include "I4ActorResource.h"

namespace i4graphics {

	I4ActorMgr::I4ActorMgr(void)
	{
	}

	I4ActorMgr::~I4ActorMgr(void)
	{
		destroy();
	}

	I4Actor* I4ActorMgr::createActor(const string& name)
	{
		I4Actor* actor = NULL;

		ActorMap::iterator itr = mapActor.find(name);
		if (itr == mapActor.end())
		{
			actor = new I4Actor;
			mapActor.insert(make_pair(name, actor));
		}
		else
		{
			actor = itr->second;
		}

		return actor;
	}

	bool I4ActorMgr::attachBone(I4Actor* actor, const char* fname)
	{
		I4ActorBoneResource* boneResource = NULL;

		ActorBoneResourceMap::iterator itr = mapActorBoneResource.find(fname);
		if (itr == mapActorBoneResource.end())
		{
			boneResource = new I4ActorBoneResource;
			if (boneResource->loadBone(fname) == false)
			{
				delete boneResource;
				return false;
			}

			mapActorBoneResource.insert(make_pair(fname, boneResource));
		}
		else
		{
			boneResource = itr->second;
		}

		return actor->registerBone(boneResource);
	}

	bool I4ActorMgr::attachMesh(I4Actor* actor, const char* fname)
	{
		I4ActorMeshResource* meshResource = NULL;

		ActorMeshResourceMap::iterator itr = mapActorMeshResource.find(fname);
		if (itr == mapActorMeshResource.end())
		{
			meshResource = new I4ActorMeshResource;
			if (meshResource->loadMesh(fname) == false)
			{
				delete meshResource;
				return false;
			}

			mapActorMeshResource.insert(make_pair(fname, meshResource));
		}
		else
		{
			meshResource = itr->second;
		}

		return actor->registerMesh(meshResource);
	}

	bool I4ActorMgr::attachAni(I4Actor* actor, const char* fname, const char* aniName)
	{
		I4ActorAniResource* aniResource = NULL;

		ActorAniResourceMap::iterator itr = mapActorAniResource.find(fname);
		if (itr == mapActorAniResource.end())
		{
			aniResource = new I4ActorAniResource;
			if (aniResource->loadAni(fname) == false)
			{
				delete aniResource;
				return false;
			}

			mapActorAniResource.insert(make_pair(fname, aniResource));
		}
		else
		{
			aniResource = itr->second;
		}

		return actor->registerAni(aniResource, aniName);
	}

	void I4ActorMgr::destroy()
	{
		for (ActorMap::iterator itr = mapActor.begin(); itr != mapActor.end(); ++itr)
		{			
			delete itr->second;
		}
		mapActor.clear();

		for (ActorBoneResourceMap::iterator itr = mapActorBoneResource.begin(); itr != mapActorBoneResource.end(); ++itr)
		{			
			delete itr->second;
		}
		mapActorBoneResource.clear();

		for (ActorMeshResourceMap::iterator itr = mapActorMeshResource.begin(); itr != mapActorMeshResource.end(); ++itr)
		{			
			delete itr->second;
		}
		mapActorMeshResource.clear();

		for (ActorAniResourceMap::iterator itr = mapActorAniResource.begin(); itr != mapActorAniResource.end(); ++itr)
		{			
			delete itr->second;
		}
		mapActorAniResource.clear();
	}

}

