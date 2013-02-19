#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"
#include "I4AABB.h"
#include "I4Hash.h"
using namespace i4core;

namespace i4graphics {

	class I4Actor;
	class I4ActorBoneResource;
	class I4ActorMeshResource;
	class I4ActorAniResource;
	class I4ActorMaterialResource;
	class I4Texture;
	class I4Mesh;

	class I4GRAPHICS_API I4ActorMgr
	{
		typedef map<string, I4Actor*>					I4ActorMap;
		typedef map<string, I4ActorBoneResource*>		I4ActorBoneResourceMap;
		typedef map<string, I4ActorMeshResource*>		I4ActorMeshResourceMap;
		typedef map<string, I4ActorMaterialResource*>	I4ActorMaterialResourceMap;
		typedef map<string, I4ActorAniResource*>		I4ActorAniResourceMap;
		
	public:
		I4ActorMgr(void);
		~I4ActorMgr(void);

		I4Actor*		createActor(const string& name);

		bool			attachBone(I4Actor* actor, const char* fname);
		bool			attachMesh(I4Actor* actor, const char* fname);
		bool			attachMaterial(I4Actor* actor, const char* fname);
		bool			attachAni(I4Actor* actor, const char* fname, const char* aniName);

		void			destroy();

	private:
		I4ActorMap					mapActor;
		I4ActorBoneResourceMap		mapActorBoneResource;
		I4ActorMeshResourceMap		mapActorMeshResource;
		I4ActorMaterialResourceMap	mapActorMaterialResource;
		I4ActorAniResourceMap		mapActorAniResource;
	};

}