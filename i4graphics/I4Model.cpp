#include "stdafx.h"
#include "I4Model.h"
#include "I4ModelResource.h"
#include "I4ModelElement.h"
#include "I4ShaderMgr.h"
#include "I4KeyFrameSet.h"
#include "I4Profile.h"
#include "I4Renderer.h"

namespace i4graphics
{

	Model::Model()
		: aniSpeed(1.0f)
		, shadowCaster(true)
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

			auto modelBone = new ModelBone(this, boneInfo);

			vecBone.push_back(modelBone);
			mapElement.insert(make_pair(boneInfo->name, modelBone));
		}

		vecSkinTM.resize(vecBone.size());

		return true;
	}

	bool Model::registerMesh(ModelMeshResource* meshResource)
	{
		set<int> setAllRefBone;
		unsigned int meshCount = meshResource->getMeshCount();
		for (unsigned int i = 0; i < meshCount; ++i)
		{
			ModelElementInfo* meshInfo = meshResource->getMeshInfo(i);
			TriangleMesh* mesh = meshResource->getMesh(i);

			auto modelMesh = new ModelMesh(this, meshInfo, mesh);

			vecMesh.push_back(modelMesh);
			mapElement.insert(make_pair(meshInfo->name, modelMesh));

			// 이 모델이 참조하는 모든 본 테이블을 만든다.
			for (unsigned int j = 0; j < meshInfo->boneRefTable.size(); ++j)
			{
				setAllRefBone.insert(meshInfo->boneRefTable[j]);
			}
		} 

		// 복사
		vecAllRefBone.assign(setAllRefBone.begin(), setAllRefBone.end());

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

	bool Model::registerAni(ModelAniResource* aniResource, const char* aniName, float start, float end)
	{
		unsigned int keyFrameSetCount = aniResource->getKeyFrameSetCount();
		for (unsigned int i = 0; i < keyFrameSetCount; ++i)
		{
			KeyFrameSet* keyFrameSet = aniResource->getKeyFrameSet(i);
			ModelElementMap::iterator itr = mapElement.find(keyFrameSet->nodeName);
			if (itr != mapElement.end())
			{
				(itr->second)->registerAni(aniName, keyFrameSet, start, end);
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
			vecBone[i]->animate(dt*aniSpeed, vecBone[i]->getParentTM());
		}

		// 이 모델이 참조하는 본들만 스킨행렬을 계산한다.
		for (unsigned int i = 0; i < vecAllRefBone.size(); ++i)
		{
			const int boneID = vecAllRefBone[i];
			vecSkinTM[boneID] = vecBone[boneID]->getSkinTM();
		}

		unsigned int meshSize = vecMesh.size();
		for (unsigned int i = 0; i < meshSize; ++i)
		{
			vecMesh[i]->animate(dt*aniSpeed, vecMesh[i]->getParentTM());
		}
	}


	void Model::commitToRenderer(Renderer* renderer, const Matrix4x4& worldTM)
	{
		PROFILE_THISFUNC;

		if (renderer->isDebugMode())
		{
			unsigned int boneSize = vecBone.size();
			for (unsigned int i = 0; i < boneSize; ++i)
			{
				vecBone[i]->commitToRenderer(renderer, worldTM);
			}
		}

		unsigned int meshSize = vecMesh.size();
		for (unsigned int i = 0; i < meshSize; ++i)
		{
			vecMesh[i]->commitToRenderer(renderer, worldTM);
		}
	}

	void Model::playAnimation(const char* aniName, float speed)
	{
		aniSpeed = speed;

		for (auto& itr : mapElement)
		{
			(itr.second)->playAni(aniName);
		}
	}

}