#pragma once

#include "I4Material.h"
#include "I4StaticMesh.h"
#include "I4ActorElementInfo.h"
#include "I4XmlData.h"
#include "I4GeometryBuffer.h"
#include "I4AABB.h"

using namespace i4core;

namespace i4graphics {

	struct ParsedData
	{
		bool				skined;
		I4Material*			material;
		/*
		I4Texture*			diffuseMap;
		I4Texture*			specularMap;
		I4Texture*			normalMap;
		*/
		I4AABB					localAABB;
		vector<I4Vector3>		vecPosition;
		vector<I4Vector3>		vecNormal;
		vector<I4Vector4>		vecTangent;
		vector<I4TextureUV>		vecVertexUV;
		vector<I4Index16>		vecVertexIndex;
		vector<I4TextureUV>		vecTexUV;
		vector<I4Index16>		vecTexIndex;
		vector<I4SkinInfo>		vecSkinInfo;
	};

	class I4Texture;
	struct KeyFrameSet;

	//-------------------- I4ActorBoneResource -----------------------

	class I4GRAPHICS_API I4ActorBoneResource
	{
		typedef vector<I4ActorElementInfo*>		I4ActorInfoVector;
	public:
		I4ActorBoneResource();
		virtual ~I4ActorBoneResource();
		
		bool					loadBone(const char* fname);

		void					destroy();

		unsigned int			getBoneCount() const				{ return vecBoneInfo.size(); }
		I4ActorElementInfo*		getBoneInfo(unsigned int i) const	{ return vecBoneInfo[i]; }

	private:
		void					parseBone(I4XmlData& xml);

		void					parseNodeInfo(I4ActorElementInfo& out, I4XmlData& xml);
		void					parseNodeInfoLocalTM(I4ActorElementInfo& out,I4XmlData& xml);
		void					parseNodeInfoWorldTM(I4ActorElementInfo& out,I4XmlData& xml);
		
	private:
		I4ActorInfoVector			vecBoneInfo;
	};

	//-------------------- I4ActorMeshResource -----------------------

	class I4GRAPHICS_API I4ActorMeshResource
	{
		typedef vector<I4Material*>			I4MaterialVector;
		typedef vector<I4Texture*>			I4TextureVector;
		typedef vector<I4ActorElementInfo*>	I4ActorInfoVector;
		typedef vector<I4StaticMesh*>		I4MeshVector;

	public:
		I4ActorMeshResource();
		virtual ~I4ActorMeshResource();

		bool					loadMesh(const char* fname);

		void					destroy();

		unsigned int			getMeshCount() const				{ return vecMeshInfo.size(); }
		I4ActorElementInfo*		getMeshInfo(unsigned int i) const	{ return vecMeshInfo[i]; }
		I4StaticMesh*			getMesh(unsigned int i) const		{ return vecMesh[i]; }

	private:
		void					parseMesh(I4XmlData& xml);

		void					parseNodeInfo(I4ActorElementInfo& out, I4XmlData& xml);
		void					parseNodeInfoLocalTM(I4ActorElementInfo& out,I4XmlData& xml);
		void					parseNodeInfoWorldTM(I4ActorElementInfo& out,I4XmlData& xml);

		void					parseMaterials(ParsedData& out, I4XmlData& xml);
		void					parseMeshVertex(ParsedData& out,I4XmlData& xml);
		void					parseMeshNormal(ParsedData& out,I4XmlData& xml);
		void					parseMeshIndex(ParsedData& out,I4XmlData& xml);		
		void					parseMeshTexUV(ParsedData& out,I4XmlData& xml);
		void					parseMeshTexIndex(ParsedData& out,I4XmlData& xml);
		bool					parseMeshWeight(ParsedData& out,I4XmlData& xml);
		void					mergeMeshTextureUV(ParsedData& out,I4XmlData& xml);

		I4StaticMesh*			buildMesh(ParsedData &ParsedData);

		void					CalculateTangentArray(long vertexCount, const vector<I4Vector3>& vertex, const vector<I4Vector3>& normal,
			const vector<I4TextureUV>& texCoord, long triangleCount, const vector<I4Index16>& triangle, vector<I4Vector4>& tangent);
	private:
		I4MaterialVector			vecMaterial;
		I4TextureVector				vecTexture;
		I4ActorInfoVector			vecMeshInfo;
		I4MeshVector				vecMesh;
	};

	//-------------------- I4ActorAniResource -----------------------

	class I4GRAPHICS_API I4ActorAniResource
	{
		typedef vector<KeyFrameSet*>			KeyFrameSetVector;
	public:
		I4ActorAniResource();
		virtual ~I4ActorAniResource();

		bool					loadAni(const char* fname);

		void					destroy();

		unsigned int			getKeyFrameSetCount() const				{ return vecKeyFrameSet.size(); }
		KeyFrameSet*			getKeyFrameSet(unsigned int i) const	{ return vecKeyFrameSet[i]; }

	private:
		void					parseAnimation(I4XmlData& xml);
		void					parseKeyFrameSet(KeyFrameSet& out, I4XmlData& xml);

	private:
		KeyFrameSetVector		vecKeyFrameSet;
	};
}