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
	class I4Texture;
	class I4StaticMesh;

	class I4MeshInstance
	{
	public:
		I4Matrix4x4		meshLocalTM;
		I4AABB			meshLocalAABB;
		I4StaticMesh*	mesh;
		I4HashID		diffuseMapID;
		I4HashID		specularMapID;
		I4HashID		normalMapID;
		float  			specularInensity;
		float			specularPower;
	};

	class I4GRAPHICS_API I4ActorMgr
	{
		typedef map<string, I4Actor*>					ActorMap;
		typedef map<string, I4ActorBoneResource*>		ActorBoneResourceMap;
		typedef map<string, I4ActorMeshResource*>		ActorMeshResourceMap;
		typedef map<string, I4ActorAniResource*>		ActorAniResourceMap;
		
	public:
		I4ActorMgr(void);
		~I4ActorMgr(void);

		I4Actor*		createActor(const string& name);

		bool			attachBone(I4Actor* actor, const char* fname);
		bool			attachMesh(I4Actor* actor, const char* fname);
		bool			attachAni(I4Actor* actor, const char* fname, const char* aniName);

		void			destroy();

		//I4Texture*		findTexture(I4HashID meshID);
		//I4StaticMesh*	findMesh(I4HashID meshID);

	private:
		ActorMap				mapActor;
		ActorBoneResourceMap	mapActorBoneResource;
		ActorMeshResourceMap	mapActorMeshResource;
		ActorAniResourceMap		mapActorAniResource;
	};

}