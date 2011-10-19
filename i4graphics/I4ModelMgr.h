#pragma once

#include "i4graphics.h"
#include "I4GeometryBuffer.h"
#include "I4Matrix4x4.h"
#include "I4Hash.h"

namespace i4graphics
{
	struct ParsedMeshData
	{
		std::vector<I4Vector3>		vecPosition;
		std::vector<I4Vector3>		vecNormal;
		std::vector<I4Vector4>		vecTangent;
		std::vector<I4TextureUV>	vecVertexUV;
		std::vector<I4Index16>		vecIndex;
		std::vector<I4TextureUV>	vecTexUV;
		std::vector<I4Index16>		vecTexIndex;
		std::vector<I4Vector4>		vecBlendIndices;
		std::vector<I4Vector4>		vecBlendWeight;
		I4Matrix4x4					localTM;
		I4Matrix4x4					worldTM;
		I4HashID					diffuseMapID;
		I4HashID					specularMapID;
		I4HashID					normalMapID;
		float  						specularInensity;
		float						specularPower;
	};

	class I4Texture;
	class I4StaticMesh;


	class I4MeshInstance
	{
	public:
		I4Matrix4x4		resultTM;
		I4Matrix4x4		meshLocalTM;
		I4Matrix4x4		meshWorldTM;
		I4HashID		meshID;
		I4HashID		diffuseMapID;
		I4HashID		specularMapID;
		I4HashID		normalMapID;
		float  			specularInensity;
		float			specularPower;
	};

	
	class I4ModelMgr;
	class I4ModelInstance;

	class I4ModelPrototype : public I4HashData<I4ModelPrototype>
	{
		friend class I4ModelMgr;
	public:
		I4ModelInstance*	createInstance();
	private:
		I4ModelPrototype()	{}

	private:
		std::vector<I4MeshInstance>		vecSubMeshInstance;		
	};

	class I4ModelInstance : public I4HashData<I4ModelInstance>
	{
		friend class I4ModelMgr;
		friend class I4ModelPrototype;
	public:		
		I4MeshInstance&		getSubMeshInstance(unsigned int i)	{ return vecSubMeshInstance[i]; }
		
		const I4Matrix4x4&	getModelTM()						{ return modelTM; }
		void				setModelTM(const I4Matrix4x4& mat)	{ modelTM = mat; }

		unsigned int		getSubCount()						{ return vecSubMeshInstance.size(); }

	private:
		I4ModelInstance()	{}

	private:
		std::vector<I4MeshInstance>		vecSubMeshInstance;
		I4Matrix4x4						modelTM;
		I4HashID						prototypeID;
	};

	class I4GRAPHICS_API I4ModelMgr
	{
		typedef std::map<I4HashID, I4ModelPrototype*>	I4ModelPrototypeMap;
		typedef std::map<I4HashID, I4ModelInstance*>	I4ModelInstanceMap;
		typedef std::map<I4HashID, I4Texture*>			I4TextureMap;
		typedef std::map<I4HashID, I4StaticMesh*>		I4StaticMeshMap;

	public:
		I4ModelMgr();
		virtual ~I4ModelMgr();

		I4ModelInstance*		createInstance(const char* modelPrototypeName, const char* modelInstanceName);
		void					destroyInstance(I4ModelInstance* modelInstance);

		I4ModelPrototype*		findModelPrototype(I4HashID prototypeID);
		I4ModelInstance*		findModelInstance(I4HashID modelInstanceID);
		I4Texture*				findTexture(I4HashID meshID);
		I4StaticMesh*			findMesh(I4HashID meshID);

	private:
		void					destroy();

	private:
		I4ModelPrototype*		buildModelPrototype(I4XmlData& xml);

		void					parseMeshInfo(ParsedMeshData& out, I4XmlData& xml);	
		void					parseMaterials(ParsedMeshData& out, I4XmlData& xml);	
		void					parseMeshVertex(ParsedMeshData& out,I4XmlData& xml);
		void					parseMeshNormal(ParsedMeshData& out,I4XmlData& xml);
		void					parseMeshIndex(ParsedMeshData& out,I4XmlData& xml);		
		void					parseMeshTexUV(ParsedMeshData& out,I4XmlData& xml);
		void					parseMeshTexIndex(ParsedMeshData& out,I4XmlData& xml);
		void					parseMeshWeight(ParsedMeshData& out,I4XmlData& xml);
		void					mergeMeshTextureUV(ParsedMeshData& out,I4XmlData& xml);
		I4StaticMesh*			buildMesh(ParsedMeshData &parsedMeshData);

	private:
		I4ModelPrototypeMap		mapModelPrototype;
		I4ModelInstanceMap		mapModelInstance;
		I4TextureMap			mapTexture;
		I4StaticMeshMap			mapStaticMesh;
	};
}