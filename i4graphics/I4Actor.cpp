#include "stdafx.h"
#include "I4Actor.h"
#include "I4ActorResource.h"
#include "I4ActorElement.h"
#include "I4ShaderMgr.h"
#include "I4KeyFrameSet.h"
#include "I4Profile.h"

namespace i4graphics
{

	I4Actor::I4Actor()
	{
	}

	I4Actor::~I4Actor()
	{
		destroy();
	}

	bool I4Actor::registerBone(I4ActorBoneResource* boneResource)
	{
		unsigned int boneCount = boneResource->getBoneCount();
		for (unsigned int i = 0; i < boneCount; ++i)
		{
			I4ActorElementInfo* boneInfo = boneResource->getBoneInfo(i);

			I4ActorBone* actorBone = new I4ActorBone(this, boneInfo);

			vecBone.push_back(actorBone);
			mapElement.insert(make_pair(boneInfo->name, actorBone));
		}

		vecSkinTM.resize(vecBone.size());

		return true;
	}

	bool I4Actor::registerMesh(I4ActorMeshResource* meshResource)
	{
		unsigned int meshCount = meshResource->getMeshCount();
		for (unsigned int i = 0; i < meshCount; ++i)
		{
			I4ActorElementInfo* meshInfo = meshResource->getMeshInfo(i);
			I4Mesh* mesh = meshResource->getMesh(i);

			I4ActorMesh* actorMesh = nullptr;
			if (mesh->skined)
			{
				actorMesh = new ActorSkinedMeshGPU(this, meshInfo, mesh);
			}
			else
			{
				actorMesh = new ActorRigidMesh(this, meshInfo, mesh);
			}

			vecMesh.push_back(actorMesh);
			mapElement.insert(make_pair(meshInfo->name, actorMesh));
		} 

		return true;
	}
	
	bool I4Actor::registerMaterial(I4ActorMaterialResource* mtrlResource)
	{
		vecSkinTM.resize(vecBone.size());

		unsigned int mtrlCount = mtrlResource->getMaterialCount();
		for (unsigned int i = 0; i < mtrlCount; ++i)
		{
			I4Material* material = mtrlResource->getMaterial(i);
			vecMesh[i]->setMaterial(material);
		} 

		return true;
	}

	bool I4Actor::registerAni(I4ActorAniResource* aniResource, const char* aniName)
	{
		unsigned int keyFrameSetCount = aniResource->getKeyFrameSetCount();
		for (unsigned int i = 0; i < keyFrameSetCount; ++i)
		{
			I4KeyFrameSet* keyFrameSet = aniResource->getKeyFrameSet(i);
			I4ActorElementMap::iterator itr = mapElement.find(keyFrameSet->nodeName);
			if (itr != mapElement.end())
			{
				(itr->second)->registerAni(aniName, keyFrameSet);
			}
		}

		return true;
	}

	bool I4Actor::initialize()
	{
		for (auto&itr : mapElement)
		{
			if ((itr.second)->initialize() == false)
				return false;
		}

		return true;
	}

	void I4Actor::destroy()
	{
		for (auto&itr : mapElement)
		{
			delete itr.second;
		}
		mapElement.clear();
	}

	I4ActorElement* I4Actor::findElement(const char* name)
	{
		auto itr = mapElement.find(name);
		if (itr != mapElement.end())
			return itr->second;

		return nullptr;
	}

	void I4Actor::animate(float deltaSec)
	{
		I4PROFILE_THISFUNC;

		unsigned int boneSize = vecBone.size();
		for (unsigned int i = 0; i < boneSize; ++i)
		{
			vecBone[i]->animate(deltaSec, vecBone[i]->getParentTM());
			vecSkinTM[i] = vecBone[i]->getSkinTM();
		}

		unsigned int meshSize = vecMesh.size();
		for (unsigned int i = 0; i < meshSize; ++i)
		{
			vecMesh[i]->animate(deltaSec, vecMesh[i]->getParentTM());
		}
	}


	void I4Actor::render(I4DeferredRenderer* renderer, const I4Matrix4x4& worldTM)
	{
		I4PROFILE_THISFUNC;

		unsigned int meshSize = vecMesh.size();
		for (unsigned int i = 0; i < meshSize; ++i)
		{
			vecMesh[i]->render(renderer, worldTM);
		}
	}

	void I4Actor::playAnimation(const char* aniName)
	{
		for (auto&itr : mapElement)
		{
			(itr.second)->playAni(aniName);
		}
	}

}