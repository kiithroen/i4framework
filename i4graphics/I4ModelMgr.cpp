#include "I4ModelMgr.h"
#include "I4Log.h"
#include "I4XmlData.h"
#include "I4Hash.h"
#include "I4VideoDriver.h"
#include "I4Texture.h"
#include "I4StaticMesh.h"
#include "I4StopWatch.h"

namespace i4graphics
{
	I4ModelInstance* I4ModelPrototype::createInstance()
	{
		I4ModelInstance* instance = new I4ModelInstance;
		instance->vecSubMeshInstance.assign(vecSubMeshInstance.begin(), vecSubMeshInstance.end());	

		return instance;
	}

	I4ModelMgr::I4ModelMgr()
	{
	}

	I4ModelMgr::~I4ModelMgr()
	{
		destroy();
	}

	I4ModelInstance* I4ModelMgr::createInstance(const char* modelPrototypeName, const char* modelInstanceName)
	{
		I4HashID modelInstanceID(modelInstanceName);

		I4ModelInstance* modelInstance = findModelInstance(modelInstanceID);
		if (modelInstance != nullptr)
		{
			if (modelInstance->sameName(modelInstanceName))
			{
				return modelInstance;
			}
			else
			{
				I4LOG_WARN << L"model instance duplicated hash id : " << modelInstance->getName().c_str() <<  L" vs " << modelInstanceName;
				return nullptr;
			}
		}

		I4HashID modelPrototypeID(modelPrototypeName);
		I4ModelPrototype* modelPrototype = findModelPrototype(modelPrototypeID);
		if (modelPrototype != nullptr)
		{
			if (modelPrototype->sameName(modelPrototypeName) == false)
			{
				I4LOG_WARN << L"model prototype duplicated hash id : " << modelPrototype->getName().c_str() <<  L" vs " << modelPrototypeName;
				return nullptr;
			}
		}
		else
		{
			I4StopWatch watch;

			I4LOG_INFO << L"parse model start : " << modelPrototypeName;

			watch.reset();

			I4XmlData xml;
			if (xml.parseFromFile(modelPrototypeName) == false)
			{
				I4LOG_WARN << L"parse model failed.";
				return nullptr;
			}

			I4LOG_INFO << L"parse model xml : " << (int)(watch.getElapsedTime()*1000.0f) << L" ms";

			watch.reset();

			modelPrototype = buildModelPrototype(xml);
			modelPrototype->setName(modelPrototypeName);
			mapModelPrototype.insert(std::make_pair(modelPrototypeID, modelPrototype));

			I4LOG_INFO << L"build model prototype : " << (int)(watch.getElapsedTime()*1000.0f) << L" ms";
		}
				
		modelInstance = modelPrototype->createInstance();
		modelInstance->setName(modelInstanceName);
		mapModelInstance.insert(std::make_pair(modelInstance->getID(), modelInstance));

		return modelInstance;
	}

	void I4ModelMgr::destroyInstance(I4ModelInstance* modelInstance)
	{
		auto itr = mapModelInstance.find(modelInstance->getID());
		if (itr != mapModelInstance.end())
		{
			assert(modelInstance->isEqual(*itr->second));

			mapModelInstance.erase(itr);
			delete modelInstance;
		}
		else
		{
			I4LOG_WARN << "unknown model instance : " << modelInstance->getName().c_str();
		}

	}

	I4ModelPrototype* I4ModelMgr::findModelPrototype(I4HashID prototypeID)
	{
		auto itr = mapModelPrototype.find(prototypeID);
		if (itr == mapModelPrototype.end())
			return nullptr;

		return itr->second;
	}

	I4ModelInstance* I4ModelMgr::findModelInstance(I4HashID modelInstanceID)
	{
		auto itr = mapModelInstance.find(modelInstanceID);
		if (itr == mapModelInstance.end())
			return nullptr;

		return itr->second;
	}

	I4Texture* I4ModelMgr::findTexture(I4HashID textureID)
	{
		auto itr = mapTexture.find(textureID);
		if (itr == mapTexture.end())
			return nullptr;

		return itr->second;
	}

	I4StaticMesh* I4ModelMgr::findMesh(I4HashID meshID)
	{
		auto itr = mapStaticMesh.find(meshID);
		if (itr == mapStaticMesh.end())
			return nullptr;

		return itr->second;
	}

	void I4ModelMgr::destroy()
	{	
		for (auto &itr : mapStaticMesh)
		{
			delete itr.second;
		}
		mapStaticMesh.clear();

		for (auto &itr : mapTexture)
		{
			delete itr.second;
		}
		mapTexture.clear();

		for (auto &itr : mapModelInstance)
		{
			delete itr.second;
		}
		mapModelInstance.clear();

		for (auto &itr : mapModelPrototype)
		{
			delete itr.second;
		}
		mapModelPrototype.clear();
	}

	I4ModelPrototype* I4ModelMgr::buildModelPrototype(I4XmlData& xml)
	{		
		const char* modelName;
		if (xml.getAttrValueByPath(modelName, "name", "model") == false)
			return nullptr;

		I4ModelPrototype* modelPrototype = nullptr;

		if (xml.selectNodeByPath("model/mesh"))
		{
			modelPrototype = new I4ModelPrototype;
			do
			{
				I4StaticMesh* mesh = nullptr;
				const char* meshName;
				xml.getAttrValue(meshName, "name");
			
				std::string strFullName(modelName);
				strFullName += ".";
				strFullName += meshName;
				I4HashID meshID(strFullName);
				mesh = findMesh(meshID);
				if (mesh != nullptr && mesh->sameName(strFullName) == false)
				{
					I4LOG_WARN << L"mesh duplicated hash id : " << mesh->getName().c_str() <<  L" vs " << strFullName.c_str();
					continue;
				}

				I4StopWatch watch;
				watch.reset();
				ParsedMeshData parsedMeshData;

				parseMeshInfo(parsedMeshData, xml);		
				I4LOG_INFO << "parse info : " << (int)(watch.getElapsedTime()*1000) << " ms";

				watch.reset();
				parseMaterials(parsedMeshData, xml);
				I4LOG_INFO << "parse material : " << (int)(watch.getElapsedTime()*1000) << " ms";

				watch.reset();
				parseMeshVertex(parsedMeshData, xml);
				parseMeshNormal(parsedMeshData, xml);
				parseMeshIndex(parsedMeshData, xml);
				parseMeshTexUV(parsedMeshData, xml);
				parseMeshTexIndex(parsedMeshData, xml);
				parseMeshWeight(parsedMeshData, xml);
				I4LOG_INFO << "parse geometry : " << (int)(watch.getElapsedTime()*1000) << " ms";

				watch.reset();
				mergeMeshTextureUV(parsedMeshData, xml);
				I4LOG_INFO << "merget vertex : " << (int)(watch.getElapsedTime()*1000) << " ms";

				watch.reset();
				mesh = buildMesh(parsedMeshData);
				mesh->setName(strFullName);
				mapStaticMesh.insert(std::make_pair(meshID, mesh));
				I4LOG_INFO << "build mesh : " << (int)(watch.getElapsedTime()*1000) << " ms";

				watch.reset();
				I4MeshInstance meshInstance;
				meshInstance.meshLocalTM = parsedMeshData.localTM;
				meshInstance.meshLocalAABB = parsedMeshData.localAABB;
				meshInstance.meshID = meshID;
				meshInstance.diffuseMapID = parsedMeshData.diffuseMapID;
				meshInstance.specularMapID = parsedMeshData.specularMapID;
				meshInstance.normalMapID = parsedMeshData.normalMapID;
				meshInstance.specularInensity = parsedMeshData.specularInensity;
				meshInstance.specularPower = parsedMeshData.specularPower;
				modelPrototype->vecSubMeshInstance.push_back(meshInstance);
				I4LOG_INFO << "make mesh instance : " << (int)(watch.getElapsedTime()*1000) << " ms";

			} while (xml.selectNextSiblingNode("mesh"));

			xml.selectParentNode();
		}

		return modelPrototype;
	}
	
	void I4ModelMgr::parseMeshInfo(ParsedMeshData& out, I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("localTM"))
		{
			if (xml.selectFirstChildNode("val") == true)
			{
				int i = 0;
				do
				{
					const char* val = nullptr;
					xml.getNodeValue(val);
					sscanf_s(val, "%f %f %f", &out.localTM.m[i][0], &out.localTM.m[i][1], &out.localTM.m[i][2]);

					++i;
				} while (xml.selectNextSiblingNode("val"));

				out.localTM._14 = out.localTM._24 = out.localTM._34 = 0.0f;
				out.localTM._44 = 1.0f;

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}
	
	void I4ModelMgr::parseMaterials(ParsedMeshData& out, I4XmlData& xml)
	{
		out.specularInensity = 1.0f;
		out.specularPower = 1.0f;

		if (xml.selectFirstChildNode("material"))
		{
			if (xml.selectFirstChildNode("diffuseMap"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				char texturePath[256] = "";
				strcat_s(texturePath, val);

				I4HashID textureID(texturePath);

				I4Texture* texture = findTexture(textureID);
				if (texture != nullptr)
				{
					if (texture->sameName(texturePath) == false)
					{
						out.diffuseMapID = textureID;
					}
					else
					{
						I4LOG_WARN << L"texture duplicated hash id : " << texture->getName().c_str() <<  L" vs " << texturePath;
					}
				}
				else
				{
					texture = I4VideoDriver::getVideoDriver()->createTexture();					
					if (texture->load(texturePath))
					{
						out.diffuseMapID = textureID;
						texture->setName(texturePath);
						mapTexture.insert(std::make_pair(textureID, texture));
					}
					else
					{
						I4LOG_WARN << L"can't find texture path : " << texturePath;
						delete texture;
					}
				}

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("specularMap"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				char texturePath[256] = "";
				strcat_s(texturePath, val);

				I4HashID textureID(texturePath);
				
				I4Texture* texture = findTexture(textureID);
				if (texture != nullptr)
				{
					if (texture->sameName(texturePath) == false)
					{
						out.specularMapID = textureID;
					}
					else
					{
						I4LOG_WARN << L"texture duplicated hash id : " << texture->getName().c_str() <<  L" vs " << texturePath;
					}
				}
				else
				{
					texture = I4VideoDriver::getVideoDriver()->createTexture();
					if (texture->load(texturePath))
					{
						out.specularMapID = textureID;
						texture->setName(texturePath);
						mapTexture.insert(std::make_pair(textureID, texture));
					}
					else
					{
						I4LOG_WARN << L"can't find texture path : " << texturePath;
						delete texture;
					}
				}

				xml.selectParentNode();
			}

			if (xml.selectFirstChildNode("normalMap"))
			{
				const char* val = nullptr;
				xml.getNodeValue(val);

				char texturePath[256] = "";
				strcat_s(texturePath, val);

				I4HashID textureID(texturePath);

				I4Texture* texture = findTexture(textureID);
				if (texture != nullptr)
				{
					if (texture->sameName(texturePath) == false)
					{
						out.normalMapID = textureID;
					}
					else
					{
						I4LOG_WARN << L"texture duplicated hash id : " << texture->getName().c_str() <<  L" vs " << texturePath;
					}
				}
				else
				{
					texture = I4VideoDriver::getVideoDriver()->createTexture();
					if (texture->load(texturePath))
					{
						out.normalMapID = textureID;
						texture->setName(texturePath);
						mapTexture.insert(std::make_pair(textureID, texture));
					}
					else
					{
						I4LOG_WARN << L"can't find texture path : " << texturePath;
						delete texture;
					}
				}

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}

	void I4ModelMgr::parseMeshVertex(ParsedMeshData& out, I4XmlData& xml)
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
					xml.getAttrValue(out.vecPosition[i].x, "x");
					xml.getAttrValue(out.vecPosition[i].y, "y");
					xml.getAttrValue(out.vecPosition[i].z, "z");

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

	void I4ModelMgr::parseMeshNormal(ParsedMeshData& out,I4XmlData& xml)
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
					xml.getAttrValue(out.vecNormal[i].x, "x");
					xml.getAttrValue(out.vecNormal[i].y, "y");
					xml.getAttrValue(out.vecNormal[i].z, "z");

					++i;

				} while (xml.selectNextSiblingNode("val"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}

	void I4ModelMgr::parseMeshIndex(ParsedMeshData& out,I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("index"))
		{

			int size;
			xml.getAttrValue(size, "count");
			out.vecIndex.resize(size);

			if (xml.selectFirstChildNode("val"))
			{
				int i = 0;
				do
				{
					xml.getAttrValue(out.vecIndex[i].i[0], "x");
					xml.getAttrValue(out.vecIndex[i].i[1], "y");
					xml.getAttrValue(out.vecIndex[i].i[2], "z");

					++i;

				} while (xml.selectNextSiblingNode("val"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}

	void I4ModelMgr::parseMeshTexUV(ParsedMeshData& out,I4XmlData& xml)
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
					xml.getAttrValue(out.vecTexUV[i].u, "u");
					xml.getAttrValue(out.vecTexUV[i].v, "v");

					++i;

				} while (xml.selectNextSiblingNode("val"));

				xml.selectParentNode();
			}


			xml.selectParentNode();
		}
	}

	void I4ModelMgr::parseMeshTexIndex(ParsedMeshData& out,I4XmlData& xml)
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
					xml.getAttrValue(out.vecTexIndex[i].i[0], "x");
					xml.getAttrValue(out.vecTexIndex[i].i[1], "y");
					xml.getAttrValue(out.vecTexIndex[i].i[2], "z");
					
					++i;

				} while (xml.selectNextSiblingNode("val"));

				xml.selectParentNode();
			}

			xml.selectParentNode();
		}
	}

	void I4ModelMgr::parseMeshWeight(ParsedMeshData& out, I4XmlData& xml)
	{
		if (xml.selectFirstChildNode("weight") == false)
			return;
		
		int size;
		xml.getAttrValue(size, "count");
		out.vecBlendIndices.resize(size);
		out.vecBlendWeight.resize(size);

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
						xml.getAttrValue(out.vecBlendIndices[i].xyzw[j], "i");
						xml.getAttrValue(out.vecBlendWeight[i].xyzw[j], "w");

						++j;

					} while (xml.selectNextSiblingNode("val") && j < 4);

					// 총합을 1.0으로 맞추기 위해 마지막값은 1.0에서 나머지들의 핪을 빼준값으로 한다.
					out.vecBlendWeight[i].xyzw[j - 1] = 1.0f;
					for (int k = 0; k < j - 1; ++k)
					{
						out.vecBlendWeight[i].xyzw[j - 1] -= out.vecBlendWeight[i].xyzw[k];
					}

					// 그리고 남은 부분 0으로 채운다.
					for (; j < 4; ++j)
					{
						out.vecBlendIndices[i].xyzw[j] = 0;
						out.vecBlendWeight[i].xyzw[j] = 0;
					}

					xml.selectParentNode();
				}

				++i;

			} while (xml.selectNextSiblingNode("vertex"));

			xml.selectParentNode();
		}
	}


	void I4ModelMgr::mergeMeshTextureUV(ParsedMeshData& out, I4XmlData& xml)
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
			out.vecVertexUV.resize(maxSize);
			if (out.vecBlendIndices.empty() == false)
			{
				out.vecBlendIndices.resize(maxSize);
				out.vecBlendWeight.resize(maxSize);
			}

			unsigned int indexSize = out.vecIndex.size();
			for (unsigned int i = 0; i < indexSize; ++i)
			{
				for (unsigned int j = 0; j < 3 ; ++j)
				{
					int vtxIdx = out.vecIndex[i].i[j];			// 정점의 인덱스
					int texUVIdx = out.vecTexIndex[i].i[j];	// 텍스처의 인덱스

					// 아직 텍스처 UV 복사가 이루어지지 않았으면 텍스처 UV 복사
					if (out.vecVertexUV[vtxIdx].u == I4TEX_UV_NA ||
						out.vecVertexUV[vtxIdx].v == I4TEX_UV_NA)
					{
						out.vecVertexUV[vtxIdx] = out.vecTexUV[texUVIdx];
					}
					else
					{
						// 이미 텍스처 UV 복사가 이루어진 정점인데 텍스처 UV가 다른 경우 새로운 정점 생성
						if (out.vecVertexUV[vtxIdx].u != out.vecTexUV[texUVIdx].u ||
							out.vecVertexUV[vtxIdx].v != out.vecTexUV[texUVIdx].v)
						{							
							out.vecIndex[i].i[j] = (unsigned short)verticeCount;	// 인덱스 재지정

							out.vecVertexUV[verticeCount] = out.vecTexUV[texUVIdx];
							out.vecPosition[verticeCount] = out.vecPosition[vtxIdx];
							out.vecNormal[verticeCount] = out.vecNormal[vtxIdx];
							if (out.vecBlendIndices.empty() == false)
							{
								out.vecBlendIndices[verticeCount] = out.vecBlendIndices[vtxIdx];
								out.vecBlendWeight[verticeCount] = out.vecBlendWeight[vtxIdx];
							}

							verticeCount++;
						}
					}
				}
			}

			// 낭비된 공간을 줄이기
			out.vecPosition.resize(verticeCount);
			out.vecNormal.resize(verticeCount);
			out.vecVertexUV.resize(verticeCount);
			if (out.vecBlendIndices.empty() == false)
			{
				out.vecBlendIndices.resize(verticeCount);
				out.vecBlendWeight.resize(verticeCount);
			}
		}

		out.vecTangent.resize(out.vecPosition.size());
		calculateTangentArray(out.vecPosition, out.vecNormal, out.vecVertexUV, out.vecIndex, out.vecTangent);
	}

	I4StaticMesh* I4ModelMgr::buildMesh(ParsedMeshData &parsedMeshData)
	{
		I4StaticMesh* mesh = new I4StaticMesh();
		/*
		if (parsedMeshData.skined)	
		{
			mesh->getVertexBuffer().create(parsedMeshData.vecPosition.size(), sizeof(Vertex_Pos_Normal_Tan_Tex_SkinInfo), VERTEXELEMENTS_POS_NORMAL_TAN_TEX_SKININFO);
			Vertex_Pos_Normal_Tan_Tex_SkinInfo* vertices = nullptr;
			mesh->getVertexBuffer().lock((void**)&vertices);
			for (unsigned int i = 0; i < parsedMeshData.vecPosition.size(); ++i)
			{
				vertices[i].position = parsedMeshData.vecPosition[i];
				vertices[i].normal = parsedMeshData.vecNormal[i];
				vertices[i].tangent = parsedMeshData.vecTangent[i];
				vertices[i].texUV = parsedMeshData.vecVertexUV[i];
				vertices[i].skinInfo = parsedMeshData.vecSkinInfo[i];
			}
			mesh->getVertexBuffer().unlock();
		}
		else
		*/
		// 절대 변할일 없으니까 아예 락걸고 복사하지말고 임시 메시데이타를 만들고 생성할때 버텍스 지정하는 식으로 바꺼보자.
		I4VertexBuffer* vertexBuffer = I4VideoDriver::getVideoDriver()->createVertexBuffer();
		vertexBuffer->create(parsedMeshData.vecPosition.size(), sizeof(I4Vertex_Pos_Normal_Tex_Tan));
		I4Vertex_Pos_Normal_Tex_Tan* vertices = nullptr;
		vertexBuffer->lock((void**)&vertices);
		for (unsigned int i = 0; i < parsedMeshData.vecPosition.size(); ++i)
		{
			vertices[i].position = parsedMeshData.vecPosition[i];
			vertices[i].normal = parsedMeshData.vecNormal[i];
			vertices[i].tangent = parsedMeshData.vecTangent[i];
			vertices[i].uv = parsedMeshData.vecVertexUV[i];
		}
		vertexBuffer->unlock();

		I4IndexBuffer* indexBuffer = I4VideoDriver::getVideoDriver()->createIndexBuffer();
		indexBuffer->create(parsedMeshData.vecIndex.size()*3, sizeof(unsigned short), (void*)&parsedMeshData.vecIndex[0]);

		mesh->setVertexBuffer(vertexBuffer);
		mesh->setIndexBuffer(indexBuffer);

		return mesh;
	}
}
