#include "stdafx.h"
#include "I4ActorResource.h"
#include "I4KeyFrameSet.h"
#include "I4VideoDriver.h"
#include "I4TextureMgr.h"
#include "I4Log.h"

namespace i4graphics
{

	I4ActorBoneResource::I4ActorBoneResource()
	{
	}

	I4ActorBoneResource::~I4ActorBoneResource()
	{
		destroy();
	}

	bool I4ActorBoneResource::loadBone(const char* fname)
	{
		I4XmlData xml;
		if (xml.parseFromFile(fname) == false)
			return false;

		parseBone(xml);

		return true;
	}

	void I4ActorBoneResource::destroy()
	{
		for (unsigned int i = 0; i < vecBoneInfo.size(); ++i)
		{
			delete vecBoneInfo[i];
		}
		vecBoneInfo.clear();
	}


	void I4ActorBoneResource::parseBone(I4XmlData& xml)
	{
		if (xml.selectNodeByPath("bone/node") == false)
			return;

		do
		{
			I4ActorElementInfo* nodeInfo = new I4ActorElementInfo();
			vecBoneInfo.push_back(nodeInfo);
			parseNodeInfo(*nodeInfo, xml);

		} while (xml.selectNextSiblingNode("node"));
	}

	void I4ActorBoneResource::parseNodeInfo(I4ActorElementInfo& out, I4XmlData& xml)
	{
		xml.getAttrValue(out.name, "name");
		xml.getAttrValue(out.parentName, "parent_name");
	 
 		parseNodeInfoLocalTM(out, xml);
 		parseNodeInfoWorldTM(out, xml);
	}

	void I4ActorBoneResource::parseNodeInfoLocalTM(I4ActorElementInfo& out,I4XmlData& xml)
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

	void I4ActorBoneResource::parseNodeInfoWorldTM(I4ActorElementInfo& out,I4XmlData& xml)
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

	//-------------------- I4ActorMeshResource -----------------------

	I4ActorMeshResource::I4ActorMeshResource()
	{
	}

	I4ActorMeshResource::~I4ActorMeshResource()
	{
		destroy();
	}

	bool I4ActorMeshResource::loadMesh(const char* fname)
	{
		I4XmlData xml;
		if (xml.parseFromFile(fname) == false)
			return false;

		parseMesh(xml);

		return true;
	}

	void I4ActorMeshResource::destroy()
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

	void I4ActorMeshResource::parseMesh(I4XmlData& xml)
	{
		if (xml.selectNodeByPath("mesh/node") == false)
			return;

		do
		{
			I4ActorElementInfo* nodeInfo = new I4ActorElementInfo();
			vecMeshInfo.push_back(nodeInfo);

			parseNodeInfo(*nodeInfo, xml);	 

			I4ParsedMeshData data;
			parseMeshVertex(data, xml);
			parseMeshNormal(data, xml);
			parseMeshIndex(data, xml);
			parseMeshTexUV(data, xml);
			parseMeshTexIndex(data, xml);
			parseMeshWeight(data, xml);
			mergeMeshTextureUV(data, xml);

			I4Mesh* mesh = buildMesh(data);
			vecMesh.push_back(mesh);

		} while (xml.selectNextSiblingNode("node"));
	}

	void I4ActorMeshResource::parseNodeInfo(I4ActorElementInfo& out, I4XmlData& xml)
	{
		xml.getAttrValue(out.name, "name");
		xml.getAttrValue(out.parentName, "parent_name");

		parseNodeInfoLocalTM(out, xml);
		parseNodeInfoWorldTM(out, xml);
	}

	void I4ActorMeshResource::parseNodeInfoLocalTM(I4ActorElementInfo& out,I4XmlData& xml)
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

	void I4ActorMeshResource::parseNodeInfoWorldTM(I4ActorElementInfo& out,I4XmlData& xml)
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

	void I4ActorMeshResource::parseMeshVertex(I4ParsedMeshData& out, I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("vertex"))
		{
			int size;
			xml.getAttrValue(size, "count");
			out.vecPosition.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				out.localAABB.init(out.vecPosition[0]);
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

	void I4ActorMeshResource::parseMeshNormal(I4ParsedMeshData& out,I4XmlData& xml)
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

	void I4ActorMeshResource::parseMeshIndex(I4ParsedMeshData& out,I4XmlData& xml)
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

	void I4ActorMeshResource::parseMeshTexUV(I4ParsedMeshData& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("texUV"))
		{
			int size;
			xml.getAttrValue(size, "count");
			out.vecTexUV.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f", &out.vecTexUV[i].u, &out.vecTexUV[i].v);

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}


			xml.selectParentNode();
		}
	}

	void I4ActorMeshResource::parseMeshTexIndex(I4ParsedMeshData& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("texIndex"))
		{
			int size;
			xml.getAttrValue(size, "count");
			out.vecTexIndex.resize(size);

			if (xml.selectFirstChildNode("a"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					int i0, i1, i2;
					sscanf_s(val, "%d %d %d", &i0, &i1, &i2);

					out.vecTexIndex[i].i[0] = (unsigned short)i0;
					out.vecTexIndex[i].i[1] = (unsigned short)i1;
					out.vecTexIndex[i].i[2] = (unsigned short)i2;

					++i;

				} while (xml.selectNextSiblingNode("a"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}

	void I4ActorMeshResource::parseMeshWeight(I4ParsedMeshData& out, I4XmlData& xml)
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


	void I4ActorMeshResource::mergeMeshTextureUV(I4ParsedMeshData& out, I4XmlData& xml)
	{
		// UV가 있으면
		if (out.vecTexUV.size() != 0 && out.vecTexIndex.size() != 0)
		{
			// 만약 정점을 늘여야한다면 넣을 위치의 시작
			unsigned int verticeCount = out.vecPosition.size();

			// 가능한 최대사이즈로 늘인다
			unsigned int maxSize = out.vecPosition.size() + out.vecIndex.size()*3;
			out.vecPosition.resize(maxSize);
			out.vecNormal.resize(maxSize);
			out.vecUV.resize(maxSize);
			if (out.skined)
			{
				out.vecBoneID.resize(maxSize);
				out.vecWeight.resize(maxSize);
			}
			unsigned int indexSize = out.vecIndex.size();
			for (unsigned int i = 0; i < indexSize; ++i)
			{
				for (unsigned int j = 0; j < 3 ; ++j)
				{
					int vtxIdx = out.vecIndex[i].i[j];			// 정점의 인덱스
					int texUVIdx = out.vecTexIndex[i].i[j];	// 텍스처의 인덱스

					// 아직 텍스처 UV 복사가 이루어지지 않았으면 텍스처 UV 복사
					if (out.vecUV[vtxIdx].u == I4TEX_UV_NA ||
						out.vecUV[vtxIdx].v == I4TEX_UV_NA)
					{
						out.vecUV[vtxIdx] = out.vecTexUV[texUVIdx];
					}
					else
					{
						// 이미 텍스처 UV 복사가 이루어진 정점인데 텍스처 UV가 다른 경우 새로운 정점 생성
						if (out.vecUV[vtxIdx].u != out.vecTexUV[texUVIdx].u ||
							out.vecUV[vtxIdx].v != out.vecTexUV[texUVIdx].v)
						{							
							out.vecIndex[i].i[j] = (unsigned short)verticeCount;	// 인덱스 재지정

							out.vecUV[verticeCount] = out.vecTexUV[texUVIdx];
							out.vecPosition[verticeCount] = out.vecPosition[vtxIdx];
							out.vecNormal[verticeCount] = out.vecNormal[vtxIdx];
							if (out.skined)
							{
								out.vecBoneID[verticeCount] = out.vecBoneID[vtxIdx];
								out.vecWeight[verticeCount] = out.vecWeight[vtxIdx];
							}

							verticeCount++;
						}
					}
				}
			}

			// 낭비된 공간을 줄이기
			out.vecPosition.resize(verticeCount);
			out.vecNormal.resize(verticeCount);
			out.vecUV.resize(verticeCount);
			if (out.skined)
			{
				out.vecBoneID.resize(verticeCount);
				out.vecWeight.resize(verticeCount);
			}
		}

		out.vecTangent.resize(out.vecPosition.size());
		CalculateTangentArray(out.vecPosition.size(), out.vecPosition, out.vecNormal,
			out.vecUV, out.vecIndex.size(), out.vecIndex, out.vecTangent);
	}

	I4Mesh* I4ActorMeshResource::buildMesh(I4ParsedMeshData &data)
	{
		I4Mesh* mesh = new I4Mesh();

		mesh->skined = data.skined;
		mesh->localAABB = data.localAABB;

		if (data.skined)	
		{
			mesh->vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
			mesh->vertexBuffer->create(data.vecPosition.size(), sizeof(I4Vertex_Pos_Normal_Tex_Tan_SkinInfo));
			I4Vertex_Pos_Normal_Tex_Tan_SkinInfo* vertices = nullptr;
			mesh->vertexBuffer->lock((void**)&vertices);
			for (unsigned int i = 0; i < data.vecPosition.size(); ++i)
			{
				vertices[i].position = data.vecPosition[i];
				vertices[i].normal = data.vecNormal[i];
				vertices[i].tangent = data.vecTangent[i];
				vertices[i].uv = data.vecUV[i];
				vertices[i].boneID = data.vecBoneID[i];
				vertices[i].weight = data.vecWeight[i];
			}
			mesh->vertexBuffer->unlock();
		}
		else
		{
			mesh->vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
			mesh->vertexBuffer->create(data.vecPosition.size(), sizeof(I4Vertex_Pos_Normal_Tex_Tan));
			I4Vertex_Pos_Normal_Tex_Tan* vertices = nullptr;
			mesh->vertexBuffer->lock((void**)&vertices);
			for (unsigned int i = 0; i < data.vecPosition.size(); ++i)
			{
				vertices[i].position = data.vecPosition[i];
				vertices[i].normal = data.vecNormal[i];
				vertices[i].tangent = data.vecTangent[i];
				vertices[i].uv = data.vecUV[i];
			}
			mesh->vertexBuffer->unlock();
		}

		mesh->indexBuffer = I4VideoDriver::getVideoDriver()->createIndexBuffer();
		mesh->indexBuffer->create(data.vecIndex.size()*3, sizeof(unsigned short));
		mesh->indexBuffer->copyFrom((void**)&data.vecIndex[0]);

		return mesh;
	}


	void I4ActorMeshResource::CalculateTangentArray(long vertexCount, const vector<I4Vector3>& vertex, const vector<I4Vector3>& normal,
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

	//-------------------- I4ActorMaterialResource -----------------------

	I4ActorMaterialResource::I4ActorMaterialResource()
	{
	}

	I4ActorMaterialResource::~I4ActorMaterialResource()
	{
		destroy();
	}

	bool I4ActorMaterialResource::loadMaterial(const char* fname)
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

	void I4ActorMaterialResource::destroy()
	{
		for (auto &itr : vecMaterial)
		{
			I4VideoDriver::getVideoDriver()->getTextureMgr()->unload(itr->diffuseMap);
			I4VideoDriver::getVideoDriver()->getTextureMgr()->unload(itr->specularMap);
			I4VideoDriver::getVideoDriver()->getTextureMgr()->unload(itr->normalMap);
			delete itr;
		}
		vecMaterial.clear();
	}

	
	void I4ActorMaterialResource::parseMaterials(I4XmlData& xml)
	{
		I4Material* material = new I4Material;
		if (xml.selectFirstChildNode("ambient"))
		{
			const char* val = nullptr;
			xml.getNodeValue(val);

			sscanf_s(val, "%f", &material->ambient);

			xml.selectParentNode();
		}

		if (xml.selectFirstChildNode("specularGlossiness"))
		{
			const char* val = nullptr;
			xml.getNodeValue(val);

			material->specularGlossiness = (float)atof(val);

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
	}

	//-------------------- I4ActorAniResource -----------------------


	I4ActorAniResource::I4ActorAniResource()
	{
	}

	I4ActorAniResource::~I4ActorAniResource()
	{
		destroy();
	}

	bool I4ActorAniResource::loadAni(const char* fname)
	{
		I4XmlData xml;
		if (xml.parseFromFile(fname) == false)
			return nullptr;

		parseAnimation(xml);

		return true;
	}

	void I4ActorAniResource::destroy()
	{
		for (unsigned int i = 0; i < vecKeyFrameSet.size(); ++i)
		{
			delete vecKeyFrameSet[i];
		}
		vecKeyFrameSet.clear();
	}

	void I4ActorAniResource::parseAnimation(I4XmlData& xml)
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

	void I4ActorAniResource::parseKeyFrameSet(I4KeyFrameSet& out, I4XmlData& xml)
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
	}
}