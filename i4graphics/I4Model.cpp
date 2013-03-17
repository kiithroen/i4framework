#include "stdafx.h"
#include "I4Model.h"
#include "I4ModelResource.h"
#include "I4ModelElement.h"
#include "I4ShaderMgr.h"
#include "I4KeyFrameSet.h"
#include "I4Profile.h"

namespace i4graphics
{

	I4Model::I4Model()
		: shadowCaster(true)
		, shadowReceiver(true)
	{
	}

	I4Model::~I4Model()
	{
		destroy();
	}

	bool I4Model::registerBone(I4ModelBoneResource* boneResource)
	{
		unsigned int boneCount = boneResource->getBoneCount();
		for (unsigned int i = 0; i < boneCount; ++i)
		{
			I4ModelElementInfo* boneInfo = boneResource->getBoneInfo(i);

			I4ModelBone* modelBone = new I4ModelBone(this, boneInfo);

			vecBone.push_back(modelBone);
			mapElement.insert(make_pair(boneInfo->name, modelBone));
		}

		vecSkinTM.resize(vecBone.size());

		return true;
	}

	bool I4Model::registerMesh(I4ModelMeshResource* meshResource)
	{
		unsigned int meshCount = meshResource->getMeshCount();
		for (unsigned int i = 0; i < meshCount; ++i)
		{
			I4ModelElementInfo* meshInfo = meshResource->getMeshInfo(i);
			I4TriangleMesh* mesh = meshResource->getMesh(i);

			I4ModelMesh* modelMesh = new I4ModelMesh(this, meshInfo, mesh);

			vecMesh.push_back(modelMesh);
			mapElement.insert(make_pair(meshInfo->name, modelMesh));
		} 

		return true;
	}
	
	bool I4Model::registerMaterial(I4ModelMaterialResource* mtrlResource)
	{
		unsigned int mtrlCount = mtrlResource->getMaterialCount();
		for (unsigned int i = 0; i < mtrlCount; ++i)
		{
			I4Material* material = mtrlResource->getMaterial(i);
			vecMesh[i]->setMaterial(material);
		} 

		return true;
	}

	bool I4Model::registerAni(I4ModelAniResource* aniResource, const char* aniName)
	{
		unsigned int keyFrameSetCount = aniResource->getKeyFrameSetCount();
		for (unsigned int i = 0; i < keyFrameSetCount; ++i)
		{
			I4KeyFrameSet* keyFrameSet = aniResource->getKeyFrameSet(i);
			I4ModelElementMap::iterator itr = mapElement.find(keyFrameSet->nodeName);
			if (itr != mapElement.end())
			{
				(itr->second)->registerAni(aniName, keyFrameSet);
			}
		}

		return true;
	}

	bool I4Model::initialize()
	{
		for (auto& itr : mapElement)
		{
			if ((itr.second)->initialize() == false)
				return false;
		}

		animate(0);

		return true;
	}

	void I4Model::destroy()
	{
		for (auto& itr : mapElement)
		{



			delete itr.second;
		}
		mapElement.clear();
	}

	I4ModelElement* I4Model::findElement(const char* name)
	{
		auto itr = mapElement.find(name);
		if (itr != mapElement.end())
			return itr->second;

		return nullptr;
	}

	void I4Model::animate(float dt)
	{
		I4PROFILE_THISFUNC;

		unsigned int boneSize = vecBone.size();
		for (unsigned int i = 0; i < boneSize; ++i)
		{
			vecBone[i]->animate(dt, vecBone[i]->getParentTM());
			vecSkinTM[i] = vecBone[i]->getSkinTM();
		}

		unsigned int meshSize = vecMesh.size();
		for (unsigned int i = 0; i < meshSize; ++i)
		{
			vecMesh[i]->animate(dt, vecMesh[i]->getParentTM());
		}
	}


	void I4Model::commitToRenderer(I4Renderer* renderer, const I4Matrix4x4& worldTM)
	{
		I4PROFILE_THISFUNC;

		unsigned int meshSize = vecMesh.size();
		for (unsigned int i = 0; i < meshSize; ++i)
		{
			vecMesh[i]->commitToRenderer(renderer, worldTM);
		}
	}

	void I4Model::playAnimation(const char* aniName)
	{
		for (auto& itr : mapElement)
		{
			(itr.second)->playAni(aniName);
		}
	}

}