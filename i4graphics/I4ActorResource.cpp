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
			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.localTM.m[i][0], &out.localTM.m[i][1], &out.localTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("val"));

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
			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.worldTM.m[i][0], &out.worldTM.m[i][1], &out.worldTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("val"));

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
		for (unsigned int i = 0; i < vecMaterial.size(); ++i)
		{
			delete vecMaterial[i];
		}
		vecMaterial.clear();

		for (unsigned int i = 0; i < vecMeshInfo.size(); ++i)
		{
			delete vecMeshInfo[i];
		}
		vecMeshInfo.clear();

		for (unsigned int i = 0; i < vecMesh.size(); ++i)
		{
			I4TextureMgr::getTextureMgr().unload(vecMesh[i]->diffuseMap);
			I4TextureMgr::getTextureMgr().unload(vecMesh[i]->specularMap);
			I4TextureMgr::getTextureMgr().unload(vecMesh[i]->normalMap);
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

			parseMaterials(data, xml);
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
			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.localTM.m[i][0], &out.localTM.m[i][1], &out.localTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("val"));

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
			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.worldTM.m[i][0], &out.worldTM.m[i][1], &out.worldTM.m[i][2]);

					++i;

				} while (xml.selectNextSiblingNode("val"));

				xml.selectParentNode();
			}

			out.worldTM._14 = out.worldTM._24 = out.worldTM._34 = 0.0f;
			out.worldTM._44 = 1.0f;

			xml.selectParentNode();
		}
	}

	void I4ActorMeshResource::parseMaterials(I4ParsedMeshData& out, I4XmlData& xml)
	{
		out.material = nullptr;
		out.diffuseMap = nullptr;
		out.specularMap = nullptr;
		out.normalMap = nullptr;

		if (xml.selectFirstChildNode("material"))
		{

			I4Material* material = new I4Material;

			if (xml.selectFirstChildNode("ambient"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				sscanf_s(val, "%f %f %f", &material->ambient.r, &material->ambient.g, &material->ambient.b);
				material->ambient.a = 1.0f;

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("diffuse"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				sscanf_s(val, "%f %f %f", &material->diffuse.r, &material->diffuse.g, &material->diffuse.b);
				material->diffuse.a = 1.0f;

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("emissive"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				float emissive = (float)atof(val);
				material->emissive.r = emissive;
				material->emissive.g = emissive;
				material->emissive.b = emissive;
				material->emissive.a = 1.0f;

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("glossiness"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				material->glossiness = (float)atof(val);

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("power"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				material->power = (float)atof(val);

				xml.selectParentNode();
			}

			vecMaterial.push_back(material);
			out.material = vecMaterial.back();

			if (xml.selectFirstChildNode("diffuseMap"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				char texturePath[256] = "texture/";
				strcat_s(texturePath, val);

				out.diffuseMap = I4TextureMgr::getTextureMgr().load(texturePath);

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("specularMap"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				char texturePath[256] = "texture/";
				strcat_s(texturePath, val);

				out.specularMap = I4TextureMgr::getTextureMgr().load(texturePath);

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("normalMap"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				char texturePath[256] = "texture/";
				strcat_s(texturePath, val);

				out.normalMap = I4TextureMgr::getTextureMgr().load(texturePath);

				xml.selectParentNode();
			}

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

			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.vecPosition[i].x, &out.vecPosition[i].y, &out.vecPosition[i].z);

					if (i == 0)
					{
						out.localAABB.init(out.vecPosition[i]);
					}
					else
					{
						out.localAABB.merge(out.vecPosition[i]);
					}

					++i;

				} while (xml.selectNextSiblingNode("val"));

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

			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f %f", &out.vecNormal[i].x, &out.vecNormal[i].y, &out.vecNormal[i].z);

					++i;

				} while (xml.selectNextSiblingNode("val"));

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
			out.vecVertexIndex.resize(size);

			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					int i0, i1, i2;
					sscanf_s(val, "%d %d %d", &i0, &i1, &i2);

					out.vecVertexIndex[i].i[0] = (unsigned short)i0;
					out.vecVertexIndex[i].i[1] = (unsigned short)i1;
					out.vecVertexIndex[i].i[2] = (unsigned short)i2;

					++i;

				} while (xml.selectNextSiblingNode("val"));

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

			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);

					sscanf_s(val, "%f %f", &out.vecTexUV[i].u, &out.vecTexUV[i].v);

					++i;

				} while (xml.selectNextSiblingNode("val"));

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

			if (xml.selectFirstChildNode("val"))
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

				} while (xml.selectNextSiblingNode("val"));

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

			if (xml.selectFirstChildNode("vertex"))
			{
				int i = 0;
				do
				{
					if (xml.selectFirstChildNode("val"))
					{
						int j = 0;
						do
						{
							const char* val = nullptr;
							xml.getNodeValue(val);

							sscanf_s(val, "%d %f", &out.vecBoneID[i].boneID[j], &out.vecWeight[i].weight[j]);

							++j;

						} while (xml.selectNextSiblingNode("val") && j < 4);

						// ������ 1.0���� ���߱� ���� ���������� 1.0���� ���������� �D�� ���ذ����� �Ѵ�.
						out.vecWeight[i].weight[j - 1] = 1.0f;
						for (int k = 0; k < j - 1; ++k)
						{
							out.vecWeight[i].weight[j - 1] -= out.vecWeight[i].weight[k];
						}

						// �׸��� ���� �κ� 0���� ä���.
						for (; j < 4; ++j)
						{
							out.vecBoneID[i].boneID[j] = 0;
							out.vecWeight[i].weight[j] = 0;
						}

						xml.selectParentNode();
					}

					++i;

				} while (xml.selectNextSiblingNode("vertex"));

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
		// UV�� ������
		if (out.vecTexUV.size() != 0 && out.vecTexIndex.size() != 0)
		{
			// ���� ������ �ÿ����Ѵٸ� ���� ��ġ�� ����
			unsigned int verticeCount = out.vecPosition.size();

			// ������ �ִ������� ���δ�
			unsigned int maxSize = out.vecPosition.size() + out.vecVertexIndex.size()*3;
			out.vecPosition.resize(maxSize);
			out.vecNormal.resize(maxSize);
			out.vecVertexUV.resize(maxSize);
			if (out.skined)
			{
				out.vecBoneID.resize(maxSize);
				out.vecWeight.resize(maxSize);
			}
			unsigned int indexSize = out.vecVertexIndex.size();
			for (unsigned int i = 0; i < indexSize; ++i)
			{
				for (unsigned int j = 0; j < 3 ; ++j)
				{
					int vtxIdx = out.vecVertexIndex[i].i[j];			// ������ �ε���
					int texUVIdx = out.vecTexIndex[i].i[j];	// �ؽ�ó�� �ε���

					// ���� �ؽ�ó UV ���簡 �̷������ �ʾ����� �ؽ�ó UV ����
					if (out.vecVertexUV[vtxIdx].u == I4TEX_UV_NA ||
						out.vecVertexUV[vtxIdx].v == I4TEX_UV_NA)
					{
						out.vecVertexUV[vtxIdx] = out.vecTexUV[texUVIdx];
					}
					else
					{
						// �̹� �ؽ�ó UV ���簡 �̷���� �����ε� �ؽ�ó UV�� �ٸ� ��� ���ο� ���� ����
						if (out.vecVertexUV[vtxIdx].u != out.vecTexUV[texUVIdx].u ||
							out.vecVertexUV[vtxIdx].v != out.vecTexUV[texUVIdx].v)
						{							
							out.vecVertexIndex[i].i[j] = (unsigned short)verticeCount;	// �ε��� ������

							out.vecVertexUV[verticeCount] = out.vecTexUV[texUVIdx];
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

			// ����� ������ ���̱�
			out.vecPosition.resize(verticeCount);
			out.vecNormal.resize(verticeCount);
			out.vecVertexUV.resize(verticeCount);
			if (out.skined)
			{
				out.vecBoneID.resize(verticeCount);
				out.vecWeight.resize(verticeCount);
			}
		}

		out.vecTangent.resize(out.vecPosition.size());
		CalculateTangentArray(out.vecPosition.size(), out.vecPosition, out.vecNormal,
			out.vecVertexUV, out.vecVertexIndex.size(), out.vecVertexIndex, out.vecTangent);
	}

	I4Mesh* I4ActorMeshResource::buildMesh(I4ParsedMeshData &data)
	{
		I4Mesh* mesh = new I4Mesh();

		mesh->skined = data.skined;
		mesh->localAABB = data.localAABB;
		mesh->diffuseMap = data.diffuseMap;
		mesh->specularMap = data.specularMap;
		mesh->normalMap = data.normalMap;

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
				vertices[i].uv = data.vecVertexUV[i];
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
				vertices[i].uv = data.vecVertexUV[i];
			}
			mesh->vertexBuffer->unlock();
		}

		mesh->indexBuffer = I4VideoDriver::getVideoDriver()->createIndexBuffer();
		mesh->indexBuffer->create(data.vecVertexIndex.size()*3, sizeof(unsigned short));
		mesh->indexBuffer->copyFrom((void**)&data.vecVertexIndex[0]);

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

			if (xml.selectFirstChildNode("val"))
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

				} while (xml.selectNextSiblingNode("val"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}

		if (xml.selectFirstChildNode("rotKey"))
		{
			int size = 0;
			xml.getAttrValue(size, "count");
			out.vecKeyRotation.resize(size);

			if (xml.selectFirstChildNode("val"))
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

				} while (xml.selectNextSiblingNode("val"));	

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}
}