#include "stdafx.h"
#include "I4ModelResource.h"
#include "I4KeyFrameSet.h"
#include "I4VideoDriver.h"
#include "I4TextureMgr.h"
#include "I4Log.h"
#include "I4Material.h"
#include "I4TriangleMesh.h"
#include "I4ModelElementInfo.h"
#include "I4XmlData.h"
#include "I4AABB.h"
#include "I4Hash.h"

namespace i4graphics
{
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
		vector<I4SubMesh>		subMeshes;
	};

	I4ModelBoneResource::I4ModelBoneResource()
	{
	}

	I4ModelBoneResource::~I4ModelBoneResource()
	{
		destroy();
	}

	bool I4ModelBoneResource::loadBone(const char* fname)
	{
		I4XmlData xml;
		if (xml.parseFromFile(fname) == false)
			return false;

		parseBone(xml);

		return true;
	}

	void I4ModelBoneResource::destroy()
	{
		for (unsigned int i = 0; i < vecBoneInfo.size(); ++i)
		{
			delete vecBoneInfo[i];
		}
		vecBoneInfo.clear();
	}


	void I4ModelBoneResource::parseBone(I4XmlData& xml)
	{
		if (xml.selectNodeByPath("bone/node") == false)
			return;

		do
		{
			I4ModelElementInfo* nodeInfo = new I4ModelElementInfo();
			vecBoneInfo.push_back(nodeInfo);
			parseNodeInfo(*nodeInfo, xml);

		} while (xml.selectNextSiblingNode("node"));
	}

	void I4ModelBoneResource::parseNodeInfo(I4ModelElementInfo& out, I4XmlData& xml)
	{
		xml.getAttrValue(out.name, "name");
		xml.getAttrValue(out.parentName, "parent_name");
	 
 		parseNodeInfoLocalTM(out, xml);
 		parseNodeInfoWorldTM(out, xml);
	}

	void I4ModelBoneResource::parseNodeInfoLocalTM(I4ModelElementInfo& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("localTM"))
		{
			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.localTM.m[i][0], &out.localTM.m[i][1], &out.localTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			out.localTM._14 = out.localTM._24 = out.localTM._34 = 0.0f;
			out.localTM._44 = 1.0f;

			xml.selectParentNode();
		}
	}

	void I4ModelBoneResource::parseNodeInfoWorldTM(I4ModelElementInfo& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("worldTM"))
		{
			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.worldTM.m[i][0], &out.worldTM.m[i][1], &out.worldTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			out.worldTM._14 = out.worldTM._24 = out.worldTM._34 = 0.0f;
			out.worldTM._44 = 1.0f;

			xml.selectParentNode();
		}
	}

	//-------------------- I4ModelMeshResource -----------------------

	I4ModelMeshResource::I4ModelMeshResource()
	{
	}

	I4ModelMeshResource::~I4ModelMeshResource()
	{
		destroy();
	}

	bool I4ModelMeshResource::loadMesh(const char* fname)
	{
		I4XmlData xml;
		if (xml.parseFromFile(fname) == false)
			return false;

		parseMesh(xml);

		return true;
	}

	void I4ModelMeshResource::destroy()
	{
		for (unsigned int i = 0; i < vecMeshInfo.size(); ++i)
		{
			delete vecMeshInfo[i];
		}
		vecMeshInfo.clear();

		for (unsigned int i = 0; i < vecMesh.size(); ++i)
		{
			
			delete vecMesh[i];
		}
		vecMesh.clear();
	}

	void I4ModelMeshResource::parseMesh(I4XmlData& xml)
	{
		if (xml.selectNodeByPath("mesh/node") == false)
			return;

		do
		{
			I4ModelElementInfo* nodeInfo = new I4ModelElementInfo();
			vecMeshInfo.push_back(nodeInfo);

			parseNodeInfo(*nodeInfo, xml);	 

			I4ParsedMeshData data;
			parseMeshPosition(data, xml);
			parseMeshNormal(data, xml);
			parseMeshUV(data, xml);
			parseMeshWeight(data, xml);
			parseMeshIndex(data, xml);
			parseMeshSub(data, xml);

			I4TriangleMesh* mesh = buildMesh(data);
			vecMesh.push_back(mesh);

		} while (xml.selectNextSiblingNode("node"));
	}

	void I4ModelMeshResource::parseNodeInfo(I4ModelElementInfo& out, I4XmlData& xml)
	{
		xml.getAttrValue(out.name, "name");
		xml.getAttrValue(out.parentName, "parent_name");

		parseNodeInfoLocalTM(out, xml);
		parseNodeInfoWorldTM(out, xml);
	}

	void I4ModelMeshResource::parseNodeInfoLocalTM(I4ModelElementInfo& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("localTM"))
		{
			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.localTM.m[i][0], &out.localTM.m[i][1], &out.localTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			out.localTM._14 = out.localTM._24 = out.localTM._34 = 0.0f;
			out.localTM._44 = 1.0f;

			xml.selectParentNode();
		}
	}

	void I4ModelMeshResource::parseNodeInfoWorldTM(I4ModelElementInfo& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("worldTM"))
		{
			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.worldTM.m[i][0], &out.worldTM.m[i][1], &out.worldTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			out.worldTM._14 = out.worldTM._24 = out.worldTM._34 = 0.0f;
			out.worldTM._44 = 1.0f;

			xml.selectParentNode();
		}
	}

	void I4ModelMeshResource::parseMeshPosition(I4ParsedMeshData& out, I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("position"))
		{
			int size;
			xml.getAttrValue(size, "count");
			out.vecPosition.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.vecPosition[i].x, &out.vecPosition[i].y, &out.vecPosition[i].z);
					
					out.localAABB.merge(out.vecPosition[i]);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}
			xml.selectParentNode();
		}
	}

	void I4ModelMeshResource::parseMeshNormal(I4ParsedMeshData& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("normal"))
		{
			int size;
			xml.getAttrValue(size, "count");
			out.vecNormal.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.vecNormal[i].x, &out.vecNormal[i].y, &out.vecNormal[i].z);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}

	void I4ModelMeshResource::parseMeshUV(I4ParsedMeshData& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("uv"))
		{
			int size;
			xml.getAttrValue(size, "count");
			out.vecUV.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f", &out.vecUV[i].u, &out.vecUV[i].v);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}


			xml.selectParentNode();
		}
	}

	void I4ModelMeshResource::parseMeshWeight(I4ParsedMeshData& out, I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("weight"))
		{
			int size;
			xml.getAttrValue(size, "count");
			out.vecBoneID.resize(size);
			out.vecWeight.resize(size);

			if (xml.selectFirstChildNode("v"))
			{
				int i = 0;
				do
				{
					if (xml.selectFirstChildNode("a"))
					{
						int j = 0;
						do
						{
							const char* val = nullptr;
							xml.getNodeValue(val);

							sscanf_s(val, "%d %f", &out.vecBoneID[i].boneID[j], &out.vecWeight[i].weight[j]);

							++j;

						} while (xml.selectNextSiblingNode("a") && j < 4);

						// 총합을 1.0으로 맞추기 위해 마지막값은 1.0에서 나머지들의 핪을 빼준값으로 한다.
						out.vecWeight[i].weight[j - 1] = 1.0f;
						for (int k = 0; k < j - 1; ++k)
						{
							out.vecWeight[i].weight[j - 1] -= out.vecWeight[i].weight[k];
						}

						// 그리고 남은 부분 0으로 채운다.
						for (; j < 4; ++j)
						{
							out.vecBoneID[i].boneID[j] = 0;
							out.vecWeight[i].weight[j] = 0;
						}

						xml.selectParentNode();
					}

					++i;

				} while (xml.selectNextSiblingNode("v"));

				xml.selectParentNode();
			}

			xml.selectParentNode();

			out.skined = true;
		}
		else
		{
			out.skined = false;
		}
	}
	
	void I4ModelMeshResource::parseMeshIndex(I4ParsedMeshData& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("index"))
		{

			int size;
			xml.getAttrValue(size, "count");
			out.vecIndex.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					int i0, i1, i2;
					sscanf_s(val, "%d %d %d", &i0, &i1, &i2);

					out.vecIndex[i].i[0] = (unsigned short)i0;
					out.vecIndex[i].i[1] = (unsigned short)i1;
					out.vecIndex[i].i[2] = (unsigned short)i2;

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}
	void I4ModelMeshResource::parseMeshSub(I4ParsedMeshData& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("sub"))
		{

			int size;
			xml.getAttrValue(size, "count");
			out.subMeshes.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					xml.getAttrValue(out.subMeshes[i].id, "id");
					xml.getAttrValue(out.subMeshes[i].start, "start");
					xml.getAttrValue(out.subMeshes[i].count, "count");
					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}

	I4TriangleMesh* I4ModelMeshResource::buildMesh(I4ParsedMeshData &data)
	{
		data.vecTangent.resize(data.vecPosition.size());
		CalculateTangentArray(data.vecPosition.size(), data.vecPosition, data.vecNormal,
			data.vecUV, data.vecIndex.size(), data.vecIndex, data.vecTangent);

		I4TriangleMesh* mesh = new I4TriangleMesh();

		mesh->skined = data.skined;
		mesh->localAABB = data.localAABB;
		mesh->subMeshes = data.subMeshes;

		if (data.skined)	
		{
			vector<I4Vertex_Pos_Normal_Tex_Tan_SkinInfo> vertices;
			vertices.resize(data.vecPosition.size());

			for (unsigned int i = 0; i < data.vecPosition.size(); ++i)
			{
				vertices[i].position = data.vecPosition[i];
				vertices[i].normal = data.vecNormal[i];
				vertices[i].tangent = data.vecTangent[i];
				vertices[i].uv = data.vecUV[i];
				vertices[i].boneID = data.vecBoneID[i];
				vertices[i].weight = data.vecWeight[i];
			}

			mesh->vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
			if (mesh->vertexBuffer->create(vertices.size(), sizeof(I4Vertex_Pos_Normal_Tex_Tan_SkinInfo), &vertices[0]) == false)
			{
				return nullptr;
			}
		}
		else
		{
			vector<I4Vertex_Pos_Normal_Tex_Tan> vertices;
			vertices.resize(data.vecPosition.size());

			for (unsigned int i = 0; i < data.vecPosition.size(); ++i)
			{
				vertices[i].position = data.vecPosition[i];
				vertices[i].normal = data.vecNormal[i];
				vertices[i].tangent = data.vecTangent[i];
				vertices[i].uv = data.vecUV[i];
			}

			mesh->vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
			mesh->vertexBuffer->create(data.vecPosition.size(), sizeof(I4Vertex_Pos_Normal_Tex_Tan), &vertices[0]);
		}

		mesh->indexBuffer = I4VideoDriver::getVideoDriver()->createIndexBuffer();
		mesh->indexBuffer->create(data.vecIndex.size()*3, sizeof(unsigned short), &data.vecIndex[0]);

		return mesh;
	}


	void I4ModelMeshResource::CalculateTangentArray(long vertexCount, const vector<I4Vector3>& vertex, const vector<I4Vector3>& normal,
		const vector<I4TextureUV>& texCoord, long triangleCount, const vector<I4Index16>& triangle, vector<I4Vector4>& tangent)
	{
		I4Vector3* tan1 = new I4Vector3[vertexCount * 2];
		I4Vector3* tan2 = tan1 + vertexCount;
		ZeroMemory(tan1, vertexCount * sizeof(I4Vector3) * 2);

		for (long a = 0; a < triangleCount; a++)
		{
			long i1 = triangle[a].i[0];
			long i2 = triangle[a].i[1];
			long i3 = triangle[a].i[2];

			const I4Vector3& v1 = vertex[i1];
			const I4Vector3& v2 = vertex[i2];
			const I4Vector3& v3 = vertex[i3];

			const I4TextureUV& w1 = texCoord[i1];
			const I4TextureUV& w2 = texCoord[i2];
			const I4TextureUV& w3 = texCoord[i3];

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.u - w1.u;
			float s2 = w3.u - w1.u;
			float t1 = w2.v - w1.v;
			float t2 = w3.v - w1.v;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			I4Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			I4Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		for (long a = 0; a < vertexCount; a++)
		{
			const I4Vector3& n = normal[a];
			const I4Vector3& t = tan1[a];

			// Gram-Schmidt orthogonalize
			I4Vector3 v = (t - n * I4Vector3::dotProduct(n, t));			
			v.normalize();
			if (_finite(v.x) == 0 || _finite(v.y) == 0 || _finite(v.z) == 0)
			{
				v.x = 0;
				v.y = 0;
				v.z = 0;
			}

			tangent[a].x = v.x;
			tangent[a].y = v.y;
			tangent[a].z = v.z;

			// Calculate handedness			
			tangent[a].w = (I4Vector3::dotProduct(I4Vector3::crossProduct(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
		}

		delete[] tan1;
	}

	//-------------------- I4ModelMaterialResource -----------------------

	I4ModelMaterialResource::I4ModelMaterialResource()
	{
	}

	I4ModelMaterialResource::~I4ModelMaterialResource()
	{
		destroy();
	}

	bool I4ModelMaterialResource::loadMaterial(const char* fname)
	{
		I4XmlData xml;
		if (xml.parseFromFile(fname) == false)
			return false;

		if (xml.selectNodeByPath("material/node") == false)
			return false;

		do
		{
			parseMaterials(xml);
		} while (xml.selectNextSiblingNode("node"));

		return true;
	}

	void I4ModelMaterialResource::destroy()
	{
		for (auto &itr : materialContainer)
		{
			for (auto& itrSub : itr)
			{
				I4VideoDriver::getVideoDriver()->getTextureMgr()->unload(itrSub->diffuseMap);
				I4VideoDriver::getVideoDriver()->getTextureMgr()->unload(itrSub->specularMap);
				I4VideoDriver::getVideoDriver()->getTextureMgr()->unload(itrSub->normalMap);
				delete itrSub;
			}
		}
		materialContainer.clear();
	}

	
	void I4ModelMaterialResource::parseMaterials(I4XmlData& xml)
	{		
		I4MaterialVector vecMaterial;

		if (xml.selectFirstChildNode("sub"))
		{
			do
			{
				I4Material* material = new I4Material;

				if (xml.selectFirstChildNode("ambient"))
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f", &material->ambient);

					xml.selectParentNode();
				}

				if (xml.selectFirstChildNode("specularLevel"))
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					material->specularLevel = (float)atof(val);

					xml.selectParentNode();
				}

				if (xml.selectFirstChildNode("specularPower"))
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					material->specularPower = (float)atof(val);

					xml.selectParentNode();
				}

				if (xml.selectFirstChildNode("diffuseMap"))
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					char texturePath[256] = "texture/";
					strcat_s(texturePath, val);

					material->diffuseMap = I4VideoDriver::getVideoDriver()->getTextureMgr()->load(texturePath);

					xml.selectParentNode();
				}

				if (xml.selectFirstChildNode("specularMap"))
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					char texturePath[256] = "texture/";
					strcat_s(texturePath, val);

					material->specularMap = I4VideoDriver::getVideoDriver()->getTextureMgr()->load(texturePath);

					xml.selectParentNode();
				}

				if (xml.selectFirstChildNode("normalMap"))
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					char texturePath[256] = "texture/";
					strcat_s(texturePath, val);

					material->normalMap = I4VideoDriver::getVideoDriver()->getTextureMgr()->load(texturePath);

					xml.selectParentNode();
				}
		
				if (xml.selectFirstChildNode("twoSide"))
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					if (strcmp(val, "true") == 0)
					{
						material->twoSide = true;
					}
					else
					{
						material->twoSide = false;
					}

					xml.selectParentNode();
				}

				vecMaterial.push_back(material);

			} while (xml.selectNextSiblingNode("sub"));

			xml.selectParentNode();
		}

		
		if (vecMaterial.empty())
		{
			// 없으면 기본 마테리얼로 최소 1개는 넣어준다.
			vecMaterial.push_back(new I4Material);
		}		
		materialContainer.push_back(vecMaterial);
	}

	//-------------------- I4ModelAniResource -----------------------


	I4ModelAniResource::I4ModelAniResource()
	{
	}

	I4ModelAniResource::~I4ModelAniResource()
	{
		destroy();
	}

	bool I4ModelAniResource::loadAni(const char* fname)
	{
		I4XmlData xml;
		if (xml.parseFromFile(fname) == false)
			return nullptr;

		parseAnimation(xml);

		return true;
	}

	void I4ModelAniResource::destroy()
	{
		for (unsigned int i = 0; i < vecKeyFrameSet.size(); ++i)
		{
			delete vecKeyFrameSet[i];
		}
		vecKeyFrameSet.clear();
	}

	void I4ModelAniResource::parseAnimation(I4XmlData& xml)
	{
		if (xml.selectNodeByPath("ani") == false)
			return;

		float startFrame = 0.0f;
		float endFrame = 0.0f;

		if (xml.selectFirstChildNode("startFrame"))
		{
			xml.getNodeValue(startFrame);

			xml.selectParentNode();
		}

		if (xml.selectFirstChildNode("endFrame"))
		{
			xml.getNodeValue(endFrame);

			xml.selectParentNode();
		}

		if (xml.selectNodeByPath("ani/node") == false)
			return;

		do
		{
			I4KeyFrameSet* keyFrameSet = new I4KeyFrameSet();
			xml.getAttrValue(keyFrameSet->nodeName, "name");
			keyFrameSet->startFrame = startFrame;
			keyFrameSet->endFrame = endFrame;

			parseKeyFrameSet(*keyFrameSet, xml);

			vecKeyFrameSet.push_back(keyFrameSet);

		} while (xml.selectNextSiblingNode("node"));
	}

	void I4ModelAniResource::parseKeyFrameSet(I4KeyFrameSet& out, I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("posKey"))
		{
			int size = 0;
			xml.getAttrValue(size, "count");
			out.vecKeyPosition.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					xml.getAttrValue(out.vecKeyPosition[i].frame,"frame");

					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f",
						&out.vecKeyPosition[i].position.x,
						&out.vecKeyPosition[i].position.y,
						&out.vecKeyPosition[i].position.z);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}

		if (xml.selectFirstChildNode("rotKey"))
		{
			int size = 0;
			xml.getAttrValue(size, "count");
			out.vecKeyRotation.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{

					xml.getAttrValue(out.vecKeyRotation[i].frame,"frame");

					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f %f",
						&out.vecKeyRotation[i].rotation.x,
						&out.vecKeyRotation[i].rotation.y,
						&out.vecKeyRotation[i].rotation.z,
						&out.vecKeyRotation[i].rotation.w);

					++i;

				} while (xml.selectNextSiblingNode("a"));	

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}

		if (xml.selectFirstChildNode("scaleKey"))
		{
			int size = 0;
			xml.getAttrValue(size, "count");
			out.vecKeyScale.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					xml.getAttrValue(out.vecKeyScale[i].frame,"frame");

					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f",
						&out.vecKeyScale[i].scale.x,
						&out.vecKeyScale[i].scale.y,
						&out.vecKeyScale[i].scale.z);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}
}