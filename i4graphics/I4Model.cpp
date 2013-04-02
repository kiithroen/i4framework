#include "stdafx.h"
#include "I4Model.h"
#include "I4ModelResource.h"
#include "I4ModelElement.h"
#include "I4ShaderMgr.h"
#include "I4KeyFrameSet.h"
#include "I4Profile.h"

namespace i4graphics
{

	Model::Model()
		: shadowCaster(true)
		, shadowReceiver(true)
	{
	}

	Model::~Model()
	{
		destroy();
	}

	bool Model::registerBone(ModelBoneResource* boneResource)
	{
		unsigned int boneCount = boneResource->getBoneCount();
		for (unsigned int i = 0; i < boneCount; ++i)
		{
			ModelElementInfo* boneInfo = boneResource->getBoneInfo(i);

			ModelBone* modelBone = new ModelBone(this, boneInfo);

			vecBone.push_back(modelBone);
			mapElement.insert(make_pair(boneInfo->name, modelBone));
		}

		vecSkinTM.resize(vecBone.size());

		return true;
	}

	bool Model::registerMesh(ModelMeshResource* meshResource)
	{
		unsigned int meshCount = meshResource->getMeshCount();
		for (unsigned int i = 0; i < meshCount; ++i)
		{
			ModelElementInfo* meshInfo = meshResource->getMeshInfo(i);
			TriangleMesh* mesh = meshResource->getMesh(i);

			ModelMesh* modelMesh = new ModelMesh(this, meshInfo, mesh);

			vecMesh.push_back(modelMesh);
			mapElement.insert(make_pair(meshInfo->name, modelMesh));
		} 

		return true;
	}
	
	bool Model::registerMaterial(ModelMaterialResource* mtrlResource)
	{
		unsigned int mtrlCount = mtrlResource->getMaterialCount();
		for (unsigned int i = 0; i < mtrlCount; ++i)
		{
			unsigned int subMeshCount = vecMesh[i]->getMesh()->subMeshes.size();
			for (unsigned int j = 0; j < subMeshCount; ++j)
			{
				Material* material = mtrlResource->getMaterial(i, j);
				vecMesh[i]->setMaterial(j, material);
			}
		} 

		return true;
	}

	bool Model::registerAni(ModelAniResource* aniResource, const char* aniName)
	{
		unsigned int keyFrameSetCount = aniResource->getKeyFrameSetCount();
		for (unsigned int i = 0; i < keyFrameSetCount; ++i)
		{
			KeyFrameSet* keyFrameSet = aniResource->getKeyFrameSet(i);
			ModelElementMap::iterator itr = mapElement.find(keyFrameSet->nodeName);
			if (itr != mapElement.end())
			{
				(itr->second)->registerAni(aniName, keyFrameSet);
			}
		}

		return true;
	}

	bool Model::initialize()
	{
		for (auto& itr : mapElement)
		{
			if ((itr.second)->initialize() == false)
				return false;
		}

		animate(0);

		return true;
	}

	void Model::destroy()
	{
		for (auto& itr : mapElement)
		{



			delete itr.second;
		}
		mapElement.clear();
	}

	ModelElement* Model::findElement(const char* name)
	{
		auto itr = mapElement.find(name);
		if (itr != mapElement.end())
			return itr->second;

		return nullptr;
	}

	void Model::animate(float dt)
	{
		PROFILE_THISFUNC;

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


	void Model::commitToRenderer(Renderer* renderer, const Matrix4x4& worldTM)
	{
		PROFILE_THISFUNC;

		unsigned int boneSize = vecBone.size();
		for (unsigned int i = 0; i < boneSize; ++i)
		{
			vecBone[i]->commitToRenderer(renderer, worldTM);
		}

		unsigned int meshSize = vecMesh.size();
		for (unsigned int i = 0; i < meshSize; ++i)
		{
			vecMesh[i]->commitToRenderer(renderer, worldTM);
		}
	}

	void Model::playAnimation(const char* aniName)
	{
		for (auto& itr : mapElement)
		{
			(itr.second)->playAni(aniName);
		}
	}

}