#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	class I4ShaderMgr;
	class I4ActorElement;
	class I4ActorBone;
	class I4ActorMesh;
	class I4ActorBoneResource;
	class I4ActorMeshResource;
	class I4ActorAniResource;
	class I4DefferedRenderer;

	class I4GRAPHICS_API I4Actor
	{
		typedef map<string, I4ActorElement*>	I4ActorElementMap;
		typedef vector<I4ActorElement*>			I4ActorElementVector;
		typedef vector<I4ActorBone*>			I4ActorBoneVector;
		typedef vector<I4ActorMesh*>			I4ActorMeshVector;
		typedef vector<I4Matrix4x4>				I4ActorSkinTMVector;
	public:
		I4Actor();
		virtual ~I4Actor();

		bool					registerBone(I4ActorBoneResource* boneResource);
		bool					registerMesh(I4ActorMeshResource* meshResource);
		bool					registerAni(I4ActorAniResource* aniResource, const char* aniName);

		bool					initialize();
		void					destroy();

		I4ActorElement*			findElement(const char* name);

		void					playAnimation(const char* aniName);

		void					animate(float deltaSec);	
		void					render(I4DefferedRenderer* renderer, const I4Matrix4x4& worldTM);

		const I4Matrix4x4&		getSkinTM(unsigned int i) const		{ return vecSkinTM[i]; }
		unsigned int			getBoneCount() const				{ return vecBone.size(); }
	private:
		I4ActorElementMap		mapElement;
		I4ActorElementVector	vecElement;
		I4ActorBoneVector		vecBone;
		I4ActorMeshVector		vecMesh;
		I4ActorSkinTMVector		vecSkinTM;
	};

}