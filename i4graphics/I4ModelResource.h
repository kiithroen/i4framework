#pragma once

#include "I4GeometryBuffer.h"

namespace i4core
{
	class XmlData;
}

using namespace i4core;

namespace i4graphics {
	
	class Texture;
	class TriangleMesh;
	struct ModelElementInfo;
	struct Material;
	struct KeyFrameSet;
	struct ParsedMeshData;

	//-------------------- ModelBoneResource -----------------------

	class ModelBoneResource
	{
		typedef vector<ModelElementInfo*>		ModelInfoVector;
	public:
		ModelBoneResource();
		virtual ~ModelBoneResource();
		
		bool					loadBone(const char* fname);

		void					destroy();

		unsigned int			getBoneCount() const				{ return vecBoneInfo.size(); }
		ModelElementInfo*		getBoneInfo(unsigned int i) const	{ return vecBoneInfo[i]; }

	private:
		void					parseBone(XmlData& xml);

		void					parseNodeInfo(ModelElementInfo& out, XmlData& xml);
		void					parseNodeInfoLocalTM(ModelElementInfo& out,XmlData& xml);
		void					parseNodeInfoWorldTM(ModelElementInfo& out,XmlData& xml);
		
	private:
		ModelInfoVector		vecBoneInfo;
	};

	//-------------------- ModelMeshResource -----------------------

	class ModelMeshResource
	{
		typedef vector<ModelElementInfo*>	ModelInfoVector;
		typedef vector<TriangleMesh*>		MeshVector;
		typedef vector<vector<int>>			MeshBoneRefTableVector;

	public:
		ModelMeshResource();
		virtual ~ModelMeshResource();

		bool					loadMesh(const char* fname);

		void					destroy();

		unsigned int			getMeshCount() const					{ return vecMeshInfo.size(); }
		ModelElementInfo*		getMeshInfo(unsigned int i) const		{ return vecMeshInfo[i]; }
		TriangleMesh*			getMesh(unsigned int i) const			{ return vecMesh[i]; }

	private:
		void					parseMesh(XmlData& xml);

		void					parseNodeInfo(ModelElementInfo& out, XmlData& xml);
		void					parseNodeInfoLocalTM(ModelElementInfo& out,XmlData& xml);
		void					parseNodeInfoWorldTM(ModelElementInfo& out,XmlData& xml);
		void					parseBoneRefTable(ModelElementInfo& out, XmlData& xml);

		void					parseMeshPosition(ParsedMeshData& out,XmlData& xml);
		void					parseMeshNormal(ParsedMeshData& out,XmlData& xml);	
		void					parseMeshUV(ParsedMeshData& out,XmlData& xml);
		void					parseMeshWeight(ParsedMeshData& out,XmlData& xml);
		void					parseMeshIndex(ParsedMeshData& out,XmlData& xml);
		void					parseMeshSub(ParsedMeshData& out,XmlData& xml);

		TriangleMesh*			buildMesh(ParsedMeshData &ParsedMeshData);

		void					CalculateTangentArray(long vertexCount, const vector<Vector3>& vertex, const vector<Vector3>& normal,
			const vector<TextureUV>& texCoord, long triangleCount, const vector<Index16>& triangle, vector<Vector4>& tangent);
	private:
		ModelInfoVector			vecMeshInfo;
		MeshVector				vecMesh;
	};

	//-------------------- ModelMaterialResource -----------------------

	class ModelMaterialResource
	{
		typedef vector<Material*>			MaterialVector;
		typedef vector<MaterialVector>	MaterialContainer;

	public:
		ModelMaterialResource();
		virtual ~ModelMaterialResource();

		bool					loadMaterial(const char* fname);

		void					destroy();

		unsigned int			getMaterialCount() const								{ return materialContainer.size(); }
		Material*				getMaterial(unsigned int i, unsigned int subID) const	{ return materialContainer[i][subID]; }

	private:
		void					parseMaterials(XmlData& xml);

	private:
		MaterialContainer		materialContainer;
	};

	//-------------------- ModelAniResource -----------------------

	class ModelAniResource
	{
		typedef vector<KeyFrameSet*>		KeyFrameSetVector;
	public:
		ModelAniResource();
		virtual ~ModelAniResource();

		bool					loadAni(const char* fname);

		void					destroy();

		unsigned int			getKeyFrameSetCount() const				{ return vecKeyFrameSet.size(); }
		KeyFrameSet*			getKeyFrameSet(unsigned int i) const	{ return vecKeyFrameSet[i]; }

	private:
		void					parseAnimation(XmlData& xml);
		void					parseKeyFrameSet(KeyFrameSet& out, XmlData& xml);

	private:
		KeyFrameSetVector		vecKeyFrameSet;
	};
}