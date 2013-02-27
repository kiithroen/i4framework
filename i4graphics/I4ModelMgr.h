#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"
#include "I4AABB.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	class I4Model;
	class I4ModelBoneResource;
	class I4ModelMeshResource;
	class I4ModelAniResource;
	class I4ModelMaterialResource;
	class I4Texture;
	class I4Mesh;

	class I4GRAPHICS_API I4ModelMgr
	{
		typedef map<string, I4Model*>					I4ModelMap;
		typedef map<string, I4ModelBoneResource*>		I4ModelBoneResourceMap;
		typedef map<string, I4ModelMeshResource*>		I4ModelMeshResourceMap;
		typedef map<string, I4ModelMaterialResource*>	I4ModelMaterialResourceMap;
		typedef map<string, I4ModelAniResource*>		I4ModelAniResourceMap;
		
	public:
		I4ModelMgr(void);
		~I4ModelMgr(void);

		I4Model*		createModel(const string& name);

		bool			attachBone(I4Model* model, const char* fname);
		bool			attachMesh(I4Model* model, const char* fname);
		bool			attachMaterial(I4Model* model, const char* fname);
		bool			attachAni(I4Model* model, const char* fname, const char* aniName);

		void			destroy();

	private:
		I4ModelMap					mapModel;
		I4ModelBoneResourceMap		mapModelBoneResource;
		I4ModelMeshResourceMap		mapModelMeshResource;
		I4ModelMaterialResourceMap	mapModelMaterialResource;
		I4ModelAniResourceMap		mapModelAniResource;
	};

}