#include "stdafx.h"
#include "I4ModelMgr.h"
#include "I4Model.h"
#include "I4ModelResource.h"
#include "I4Log.h"

namespace i4graphics {

	ModelMgr::ModelMgr(void)
	{
	}

	ModelMgr::~ModelMgr(void)
	{
		destroy();
	}

	Model* ModelMgr::createModel(const string& name)
	{
		Model* model = nullptr;

		auto itr = mapModel.find(name);
		if (itr == mapModel.end())
		{
			model = new Model;
			mapModel.insert(make_pair(name, model));
		}
		else
		{
			LOG_WARN << "duplicated model name : " << name.c_str();
		}

		return model;
	}

	bool ModelMgr::attachBone(Model* model, const char* fname)
	{
		ModelBoneResource* boneResource = nullptr;

		auto itr = mapModelBoneResource.find(fname);
		if (itr == mapModelBoneResource.end())
		{
			boneResource = new ModelBoneResource;
			if (boneResource->loadBone(fname) == false)
			{
				LOG_WARN << "model bone can't load : " << fname;
				delete boneResource;
				return false;
			}

			mapModelBoneResource.insert(make_pair(fname, boneResource));
		}
		else
		{
			boneResource = itr->second;
		}

		return model->registerBone(boneResource);
	}

	bool ModelMgr::attachMesh(Model* model, const char* fname)
	{
		ModelMeshResource* meshResource = nullptr;

		auto itr = mapModelMeshResource.find(fname);
		if (itr == mapModelMeshResource.end())
		{
			meshResource = new ModelMeshResource;
			if (meshResource->loadMesh(fname) == false)
			{
				LOG_WARN << "model mesh can't load : " << fname;
				delete meshResource;
				return false;
			}

			mapModelMeshResource.insert(make_pair(fname, meshResource));
		}
		else
		{
			meshResource = itr->second;
		}

		return model->registerMesh(meshResource);
	}

	bool ModelMgr::attachMaterial(Model* model, const char* fname)
	{
		ModelMaterialResource* mtrlResource = nullptr;

		auto itr = mapModelMaterialResource.find(fname);
		if (itr == mapModelMaterialResource.end())
		{
			mtrlResource = new ModelMaterialResource;
			if (mtrlResource->loadMaterial(fname) == false)
			{
				LOG_WARN << "model material can't load : " << fname;
				delete mtrlResource;
				return false;
			}

			mapModelMaterialResource.insert(make_pair(fname, mtrlResource));
		}
		else
		{
			mtrlResource = itr->second;
		}

		return model->registerMaterial(mtrlResource);
	}

	bool ModelMgr::attachAni(Model* model, const char* fname, const char* aniName, float start, float end, ANIMATION_PLAY_TYPE type)
	{
		ModelAniResource* aniResource = nullptr;

		auto itr = mapModelAniResource.find(fname);
		if (itr == mapModelAniResource.end())
		{
			aniResource = new ModelAniResource;
			if (aniResource->loadAni(fname) == false)
			{
				LOG_WARN << "model material can't load : " << fname;
				delete aniResource;
				return false;
			}

			mapModelAniResource.insert(make_pair(fname, aniResource));
		}
		else
		{
			aniResource = itr->second;
		}

		return model->registerAni(aniResource, aniName, start, end, type);
	}

	void ModelMgr::destroy()
	{
		for (auto& itr : mapModel)
		{			
			delete itr.second;
		}
		mapModel.clear();

		for (auto& itr : mapModelBoneResource)
		{			
			delete itr.second;
		}
		mapModelBoneResource.clear();

		for (auto& itr : mapModelMaterialResource)
		{			
			delete itr.second;
		}
		mapModelMaterialResource.clear();

		for (auto& itr : mapModelMeshResource)
		{			
			delete itr.second;
		}
		mapModelMeshResource.clear();

		for (auto& itr : mapModelAniResource)
		{			
			delete itr.second;
		}
		mapModelAniResource.clear();
	}

}

