#pragma once

#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	class I4ShaderMgr;
	class I4ModelElement;
	class I4ModelBone;
	class I4ModelMesh;
	class I4ModelBoneResource;
	class I4ModelMeshResource;
	class I4ModelMaterialResource;
	class I4ModelAniResource;
	class I4DeferredRenderer;

	class I4GRAPHICS_API I4Model
	{
		typedef map<string, I4ModelElement*>	I4ModelElementMap;
		typedef vector<I4ModelBone*>			I4ModelBoneVector;
		typedef vector<I4ModelMesh*>			I4ModelMeshVector;
		typedef vector<I4Matrix4x4>				I4ModelSkinTMVector;
	public:
		I4Model();
		virtual ~I4Model();

		bool					registerBone(I4ModelBoneResource* boneResource);
		bool					registerMesh(I4ModelMeshResource* meshResource);
		bool					registerMaterial(I4ModelMaterialResource* mtrlResource);
		bool					registerAni(I4ModelAniResource* aniResource, const char* aniName);

		bool					initialize();
		void					destroy();

		I4ModelElement*			findElement(const char* name);

		void					playAnimation(const char* aniName);

		void					animate(float deltaSec);	
		void					render(I4DeferredRenderer* renderer, const I4Matrix4x4& worldTM);

		const I4Matrix4x4&		getSkinTM(unsigned int i) const		{ return vecSkinTM[i]; }

		unsigned int			getBoneCount() const				{ return vecBone.size(); }
		I4ModelBone*			getBone(unsigned int i)				{ return vecBone[i]; }

		unsigned int			getMeshCount() const				{ return vecMesh.size(); }
		I4ModelMesh*			getMesh(unsigned int i)				{ return vecMesh[i]; }

		void					setShadowCaster(bool enable)		{ shadowCaster = enable; }
		bool					isShadowCaster() const				{ return shadowCaster; }
		
	private:
		I4ModelElementMap		mapElement;
		I4ModelBoneVector		vecBone;
		I4ModelMeshVector		vecMesh;
		I4ModelSkinTMVector		vecSkinTM;
		bool					shadowCaster;
	};

}