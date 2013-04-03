#pragma once

#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	class ShaderMgr;
	class ModelElement;
	class ModelBone;
	class ModelMesh;
	class ModelBoneResource;
	class ModelMeshResource;
	class ModelMaterialResource;
	class ModelAniResource;
	class Renderer;

	class Model
	{
		typedef map<string, ModelElement*>	ModelElementMap;
		typedef vector<ModelBone*>			ModelBoneVector;
		typedef vector<ModelMesh*>			ModelMeshVector;
		typedef vector<Matrix4x4>			ModelSkinTMVector;
	public:
		Model();
		virtual ~Model();

		bool					registerBone(ModelBoneResource* boneResource);
		bool					registerMesh(ModelMeshResource* meshResource);
		bool					registerMaterial(ModelMaterialResource* mtrlResource);
		bool					registerAni(ModelAniResource* aniResource, const char* aniName);

		bool					initialize();
		void					destroy();

		ModelElement*			findElement(const char* name);

		void					playAnimation(const char* aniName);

		void					animate(float dt);	
		void					commitToRenderer(Renderer* renderer, const Matrix4x4& worldTM);

		const Matrix4x4&		getSkinTMs(unsigned int i) const	{ return vecSkinTM[i]; }
		Matrix4x4*				getSkinTMs()						{ return &vecSkinTM[0]; }

		unsigned int			getBoneCount() const				{ return vecBone.size(); }
		ModelBone*				getBone(unsigned int i)				{ return vecBone[i]; }

		unsigned int			getMeshCount() const				{ return vecMesh.size(); }
		ModelMesh*				getMesh(unsigned int i)				{ return vecMesh[i]; }

		void					setShadowCaster(bool enable)		{ shadowCaster = enable; }
		bool					isShadowCaster() const				{ return shadowCaster; }

		void					setShadowReceiver(bool enable)		{ shadowReceiver = enable; }
		bool					isShadowReceiver() const			{ return shadowReceiver; }

	private:
		ModelElementMap			mapElement;
		ModelBoneVector			vecBone;
		ModelMeshVector			vecMesh;
		ModelSkinTMVector		vecSkinTM;
		vector<int>				vecAllRefBone;
		bool					shadowCaster;
		bool					shadowReceiver;
	};

}