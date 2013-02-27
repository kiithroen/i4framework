#include "stdafx.h"
#include "I4ModelMgr.h"
#include "I4Model.h"
#include "I4ModelResource.h"

namespace i4graphics {

	I4ModelMgr::I4ModelMgr(void)
	{
	}

	I4ModelMgr::~I4ModelMgr(void)
	{
		destroy();
	}

	I4Model* I4ModelMgr::createModel(const string& name)
	{
		I4Model* model = nullptr;

		auto itr = mapModel.find(name);
		if (itr == mapModel.end())
		{
			model = new I4Model;
			mapModel.insert(make_pair(name, model));
		}
		else
		{
			model = itr->second;
		}

		return model;
	}

	bool I4ModelMgr::attachBone(I4Model* model, const char* fname)
	{
		I4ModelBoneResource* boneResource = nullptr;

		auto itr = mapModelBoneResource.find(fname);
		if (itr == mapModelBoneResource.end())
		{
			boneResource = new I4ModelBoneResource;
			if (boneResource->loadBone(fname) == false)
			{
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

	bool I4ModelMgr::attachMesh(I4Model* model, const char* fname)
	{
		I4ModelMeshResource* meshResource = nullptr;

		auto itr = mapModelMeshResource.find(fname);
		if (itr == mapModelMeshResource.end())
		{
			meshResource = new I4ModelMeshResource;
			if (meshResource->loadMesh(fname) == false)
			{
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

	bool I4ModelMgr::attachMaterial(I4Model* model, const char* fname)
	{
		I4ModelMaterialResource* mtrlResource = nullptr;

		auto itr = mapModelMaterialResource.find(fname);
		if (itr == mapModelMaterialResource.end())
		{
			mtrlResource = new I4ModelMaterialResource;
			if (mtrlResource->loadMaterial(fname) == false)
			{
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

	bool I4ModelMgr::attachAni(I4Model* model, const char* fname, const char* aniName)
	{
		I4ModelAniResource* aniResource = nullptr;

		auto itr = mapModelAniResource.find(fname);
		if (itr == mapModelAniResource.end())
		{
			aniResource = new I4ModelAniResource;
			if (aniResource->loadAni(fname) == false)
			{
				delete aniResource;
				return false;
			}

			mapModelAniResource.insert(make_pair(fname, aniResource));
		}
		else
		{
			aniResource = itr->second;
		}

		return model->registerAni(aniResource, aniName);
	}

	void I4ModelMgr::destroy()
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

