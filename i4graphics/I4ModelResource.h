#pragma once

#include "I4Material.h"
#include "I4TriangleMesh.h"
#include "I4ModelElementInfo.h"
#include "I4XmlData.h"
#include "I4GeometryBuffer.h"
#include "I4AABB.h"
#include "I4Hash.h"

using namespace i4core;

namespace i4graphics {
	
	class I4Texture;
	struct I4KeyFrameSet;

	struct I4ParsedMeshData
	{
		bool					skined;
		I4AABB					localAABB;
		vector<I4Vector3>		vecPosition;
		vector<I4Vector3>		vecNormal;
		vector<I4Vector4>		vecTangent;
		vector<I4TextureUV>		vecUV;
		vector<I4Index16>		vecIndex;
		vector<I4BoneID>		vecBoneID;
		vector<I4Weight>		vecWeight;
	};

	//-------------------- I4ModelBoneResource -----------------------

	class I4ModelBoneResource
	{
		typedef vector<I4ModelElementInfo*>		I4ModelInfoVector;
	public:
		I4ModelBoneResource();
		virtual ~I4ModelBoneResource();
		
		bool					loadBone(const char* fname);

		void					destroy();

		unsigned int			getBoneCount() const				{ return vecBoneInfo.size(); }
		I4ModelElementInfo*		getBoneInfo(unsigned int i) const	{ return vecBoneInfo[i]; }

	private:
		void					parseBone(I4XmlData& xml);

		void					parseNodeInfo(I4ModelElementInfo& out, I4XmlData& xml);
		void					parseNodeInfoLocalTM(I4ModelElementInfo& out,I4XmlData& xml);
		void					parseNodeInfoWorldTM(I4ModelElementInfo& out,I4XmlData& xml);
		
	private:
		I4ModelInfoVector		vecBoneInfo;
	};

	//-------------------- I4ModelMeshResource -----------------------

	class I4ModelMeshResource
	{
		typedef vector<I4ModelElementInfo*>	I4ModelInfoVector;
		typedef vector<I4TriangleMesh*>				I4MeshVector;

	public:
		I4ModelMeshResource();
		virtual ~I4ModelMeshResource();

		bool					loadMesh(const char* fname);

		void					destroy();

		unsigned int			getMeshCount() const				{ return vecMeshInfo.size(); }
		I4ModelElementInfo*		getMeshInfo(unsigned int i) const	{ return vecMeshInfo[i]; }
		I4TriangleMesh*					getMesh(unsigned int i) const		{ return vecMesh[i]; }

	private:
		void					parseMesh(I4XmlData& xml);

		void					parseNodeInfo(I4ModelElementInfo& out, I4XmlData& xml);
		void					parseNodeInfoLocalTM(I4ModelElementInfo& out,I4XmlData& xml);
		void					parseNodeInfoWorldTM(I4ModelElementInfo& out,I4XmlData& xml);

		void					parseMeshPosition(I4ParsedMeshData& out,I4XmlData& xml);
		void					parseMeshNormal(I4ParsedMeshData& out,I4XmlData& xml);	
		void					parseMeshUV(I4ParsedMeshData& out,I4XmlData& xml);
		void					parseMeshWeight(I4ParsedMeshData& out,I4XmlData& xml);
		void					parseMeshIndex(I4ParsedMeshData& out,I4XmlData& xml);

		I4TriangleMesh*			buildMesh(I4ParsedMeshData &I4ParsedMeshData);

		void					CalculateTangentArray(long vertexCount, const vector<I4Vector3>& vertex, const vector<I4Vector3>& normal,
			const vector<I4TextureUV>& texCoord, long triangleCount, const vector<I4Index16>& triangle, vector<I4Vector4>& tangent);
	private:
		I4ModelInfoVector		vecMeshInfo;
		I4MeshVector			vecMesh;
	};

	//-------------------- I4ModelMaterialResource -----------------------

	class I4ModelMaterialResource
	{
		typedef vector<I4Material*>			I4MaterialVector;

	public:
		I4ModelMaterialResource();
		virtual ~I4ModelMaterialResource();

		bool					loadMaterial(const char* fname);

		void					destroy();

		unsigned int			getMaterialCount() const			{ return vecMaterial.size(); }
		I4Material*				getMaterial(unsigned int i) const	{ return vecMaterial[i]; }

	private:
		void					parseMaterials(I4XmlData& xml);

	private:
		I4MaterialVector		vecMaterial;
	};

	//-------------------- I4ModelAniResource -----------------------

	class I4ModelAniResource
	{
		typedef vector<I4KeyFrameSet*>		I4KeyFrameSetVector;
	public:
		I4ModelAniResource();
		virtual ~I4ModelAniResource();

		bool					loadAni(const char* fname);

		void					destroy();

		unsigned int			getKeyFrameSetCount() const				{ return vecKeyFrameSet.size(); }
		I4KeyFrameSet*			getKeyFrameSet(unsigned int i) const	{ return vecKeyFrameSet[i]; }

	private:
		void					parseAnimation(I4XmlData& xml);
		void					parseKeyFrameSet(I4KeyFrameSet& out, I4XmlData& xml);

	private:
		I4KeyFrameSetVector		vecKeyFrameSet;
	};
}