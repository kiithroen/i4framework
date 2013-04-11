#pragma once

#include "I4Matrix4x4.h"
#include "I4AABB.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	class Model;
	class ModelBoneResource;
	class ModelMeshResource;
	class ModelAniResource;
	class ModelMaterialResource;
	class Texture;
	class TriangleMesh;

	class ModelMgr
	{
		typedef map<string, Model*>					ModelMap;
		typedef map<string, ModelBoneResource*>		ModelBoneResourceMap;
		typedef map<string, ModelMeshResource*>		ModelMeshResourceMap;
		typedef map<string, ModelMaterialResource*>	ModelMaterialResourceMap;
		typedef map<string, ModelAniResource*>		ModelAniResourceMap;
		
	public:
		ModelMgr(void);
		~ModelMgr(void);

		Model*		createModel(const string& name);

		bool			attachBone(Model* model, const char* fname);
		bool			attachMesh(Model* model, const char* fname);
		bool			attachMaterial(Model* model, const char* fname);
		bool			attachAni(Model* model, const char* fname, const char* aniName, float start, float end);

		void			destroy();

	private:
		ModelMap					mapModel;
		ModelBoneResourceMap		mapModelBoneResource;
		ModelMeshResourceMap		mapModelMeshResource;
		ModelMaterialResourceMap	mapModelMaterialResource;
		ModelAniResourceMap		mapModelAniResource;
	};

}