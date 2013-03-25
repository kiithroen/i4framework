// i4converter.cpp : Defines the entry point for the console application.
//

//-----------------------------------------------------------------------------------------------------------------
// 모든 FBX 맵핑, 레퍼런스 모드를 지원하지 않고 일단 이전에 쓰던 포멧형태로 뽑혀진거만 지원한다
// 이유는 특정 방법으로 뽑으면 버텍스 크기가 너무 커지기 때문..
// 추후에 경고를 표시해주던지 다른 방법으로 변환을 하던지 해야겠다.
// TODO : 에니메이션 뽑는중... 일단 보류.
//-------------------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <fbxsdk.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

enum ExportType
{
	EXPORT_MESH = 0,
	EXPORT_MTRL,
	EXPORT_BONE,
	EXPORT_ANI,
};

FbxScene* pScene = nullptr;

// eMayaZUp,			/*!< UpVector = ZAxis, FrontVector = -ParityOdd, CoordSystem = RightHanded */
// eMayaYUp,			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = RightHanded */
// eMax,				/*!< UpVector = ZAxis, FrontVector = -ParityOdd, CoordSystem = RightHanded */
// eMotionBuilder,		/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = RightHanded */
// eOpenGL,			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = RightHanded */
// eDirectX,			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = LeftHanded */
// eLightwave			/*!< UpVector = YAxis, FrontVector =  ParityOdd, CoordSystem = LeftHanded */

FbxAxisSystem SceneAxisSystem;

FILE* fpMesh = nullptr;
FILE* fpMtrl = nullptr;
FILE* fpBone = nullptr;
FILE* fpAni = nullptr;

map<string, int> mapBoneNameList;
map<string, FbxAMatrix> mapBoneBindPoseWorld;
map<string, FbxAMatrix> mapBoneBindPoseLocal;

struct TriIndex
{
	TriIndex() {}
	TriIndex(int _i0, int _i1, int _i2)
	{
		i[0] = _i0;
		i[1] = _i1;
		i[2] = _i2;
	}

	int i[3];
};

#define TEX_UV_NA 9999.0f
	
struct TextureUV
{
	TextureUV()
		: u(TEX_UV_NA)
		, v(TEX_UV_NA)
	{
	}

	TextureUV(float _u, float _v)
		: u(_u)
		, v(_v)
	{
	}

	float u, v;
};

struct SkinInfo
{
	int boneID;
	float boneWeight;
};

struct SkinData
{
	vector<SkinInfo>	data;
};

struct MeshData
{
	vector<FbxVector4>		vecPosition;
	vector<FbxVector4>		vecNormal;
	vector<TextureUV>		vecUV;
	vector<TriIndex>		vecIndex;
	vector<TextureUV>		vecTexUV;
	vector<TriIndex>		vecTexIndex;
	vector<SkinData>		vecSkinInfo;
	vector<int>				vecMtrlID;
};

bool IsZUpRightHanded()
{
	return (SceneAxisSystem == FbxAxisSystem::eMax || SceneAxisSystem == FbxAxisSystem::eMayaZUp);
}

bool IsYUpRightHanded()
{
	return (SceneAxisSystem == FbxAxisSystem::eMayaYUp || SceneAxisSystem == FbxAxisSystem::eMotionBuilder || SceneAxisSystem == FbxAxisSystem::eOpenGL);
}

bool IsYUpLeftHanded()
{
	return (SceneAxisSystem == FbxAxisSystem::eDirectX || SceneAxisSystem == FbxAxisSystem::eLightwave);
}

float ConvertToGrayscale(const FbxDouble3& col)
{
	return col.mData[0]*0.3f + col.mData[1]*0.59f + col.mData[2]*0.11f;
}

TextureUV FbxUVToI4(const TextureUV& v)
{
	return TextureUV(v.u, 1.0f - v.v);
}

FbxVector4 FbxVectorToI4(const FbxVector4& v)
{
	if (IsZUpRightHanded())
	{
		return FbxVector4(v.mData[0], v.mData[2], v.mData[1]);
	}
	else if (IsYUpRightHanded())
	{
		return FbxVector4(v.mData[0], v.mData[1], -v.mData[2]);
	}
	else
	{
		return FbxVector4(v.mData[0], v.mData[1], v.mData[2]);
	}
}

FbxVector4 FbxNormalToI4(const FbxVector4& v)
{
	if (IsZUpRightHanded() || IsYUpRightHanded())
	{
		return FbxVector4(v.mData[0], v.mData[2], v.mData[1]);
	}
	else
	{
		return FbxVector4(v.mData[0], v.mData[1], v.mData[2]);
	}
}

FbxVector4 FbxScaleToI4(const FbxVector4& v)
{
	if (IsZUpRightHanded() || IsYUpRightHanded())
	{
		return FbxVector4(v.mData[0], v.mData[2], v.mData[1]);
	}
	else
	{
		return FbxVector4(v.mData[0], v.mData[1], v.mData[2]);
	}
}


TriIndex TriIndexToI4(const TriIndex& i)
{
	if (IsZUpRightHanded() || IsYUpRightHanded())
	{
		return TriIndex(i.i[0], i.i[2], i.i[1]);
	}
	else
	{
		return TriIndex(i.i[0], i.i[1], i.i[2]);
	}
}

FbxAMatrix FbxMatrixToI4(const FbxAMatrix& m)
{
	if (IsZUpRightHanded())
	{
		FbxAMatrix ret;
		ret.SetRow(0, FbxVectorToI4(m.GetRow(0)));
		ret.SetRow(1, FbxVectorToI4(m.GetRow(2)));
		ret.SetRow(2, FbxVectorToI4(m.GetRow(1)));
		ret.SetRow(3, FbxVectorToI4(m.GetRow(3)));

		return ret;
	}
	else if (IsYUpRightHanded())
	{
		FbxAMatrix ret;
		ret.SetRow(0, FbxVectorToI4(m.GetRow(0)));
		ret.SetRow(1, FbxVectorToI4(m.GetRow(1)));
		ret.SetRow(2, -FbxVectorToI4(m.GetRow(2)));
		ret.SetRow(3, FbxVectorToI4(m.GetRow(3)));

		return ret;
	}
	else
	{
		FbxAMatrix ret;
		ret.SetRow(0, FbxVectorToI4(m.GetRow(0)));
		ret.SetRow(1, FbxVectorToI4(m.GetRow(1)));
		ret.SetRow(2, FbxVectorToI4(m.GetRow(2)));
		ret.SetRow(3, FbxVectorToI4(m.GetRow(3)));

		return ret;
	}
}

FbxQuaternion FbxQuatToI4(const FbxQuaternion& q)
{
	if (IsZUpRightHanded())
	{
		return FbxQuaternion(-q.mData[0], -q.mData[2], -q.mData[1], q.mData[3]);
	}
	else if (IsYUpRightHanded())
	{
		return FbxQuaternion(-q.mData[0], -q.mData[1], q.mData[2], q.mData[3]);
	}
	else
	{
		return FbxQuaternion(q.mData[0], q.mData[1], q.mData[2], q.mData[3]);
	}
}

FbxAMatrix GetGeometricTransform(FbxNode *pNode)
{
	FbxVector4 t = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 r = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 s = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(t, r, s);
}

FbxAMatrix GetLocalTransform(FbxNode *pNode)
{
	return pNode->EvaluateLocalTransform()*GetGeometricTransform(pNode);
}

FbxAMatrix GetWorldTransform(FbxNode *pNode)
{
	return pNode->EvaluateGlobalTransform()*GetGeometricTransform(pNode);
}

void mergeMeshTextureUV(MeshData& out)
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
		if (out.vecSkinInfo.size() > 0)
		{
			out.vecSkinInfo.resize(maxSize);
		}
		unsigned int indexSize = out.vecIndex.size();
		for (unsigned int i = 0; i < indexSize; ++i)
		{
			for (unsigned int j = 0; j < 3 ; ++j)
			{
				int vtxIdx = out.vecIndex[i].i[j];			// 정점의 인덱스
				int texUVIdx = out.vecTexIndex[i].i[j];	// 텍스처의 인덱스

				// 아직 텍스처 UV 복사가 이루어지지 않았으면 텍스처 UV 복사
				if (out.vecUV[vtxIdx].u == TEX_UV_NA ||
					out.vecUV[vtxIdx].v == TEX_UV_NA)
				{
					out.vecUV[vtxIdx] = out.vecTexUV[texUVIdx];
				}
				else
				{
					// 이미 텍스처 UV 복사가 이루어진 정점인데 텍스처 UV가 다른 경우 새로운 정점을 생성하고 UV를 제외한 정점정보를 복사해온다.
					if (out.vecUV[vtxIdx].u != out.vecTexUV[texUVIdx].u ||
						out.vecUV[vtxIdx].v != out.vecTexUV[texUVIdx].v)
					{							
						out.vecIndex[i].i[j] = (unsigned short)verticeCount;	// 인덱스 재지정

						out.vecUV[verticeCount] = out.vecTexUV[texUVIdx];
						out.vecPosition[verticeCount] = out.vecPosition[vtxIdx];
						out.vecNormal[verticeCount] = out.vecNormal[vtxIdx];
						if (out.vecSkinInfo.size() > 0)
						{
							out.vecSkinInfo[verticeCount] = out.vecSkinInfo[vtxIdx];
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
		if (out.vecSkinInfo.size() > 0)
		{
			out.vecSkinInfo.resize(verticeCount);
		}
	}
}

void WriteMesh(FbxMesh* pMesh, FILE* fpMesh) 
{
	MeshData data;

	int lControlPointsCount = pMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();

	// 참조하고 있는 모든 정점
	data.vecPosition.resize(lControlPointsCount);
	for (int i = 0; i < lControlPointsCount; i++)
	{
		data.vecPosition[i] = FbxVectorToI4(lControlPoints[i]);
	}

	if (pMesh->GetElementNormalCount())                        
	{
		// 정점이 참조하고 있는 노말
		data.vecNormal.resize(lControlPointsCount);
		for(int i = 0; i < pMesh->GetPolygonCount(); i++)
		{
			for(int j = 0; j < 3; j++) 
			{
				FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(0);

				// 버텍스 인덱스로부터 정점 인덱스를 가져온다.
				int idx = pMesh->GetPolygonVertex(i, j);

				// 그리고 정점인덱스에 해당하는 노말값을 가져와서 집어넣는다.
				data.vecNormal[idx] = FbxNormalToI4(leNormals->GetDirectArray().GetAt(i*3 + j));
			}
		}
	}

	// 각 폴리곤 인덱스마다 마테리얼아이디값을 저장해둔다.
	FbxGeometryElementMaterial* pMaterial = pMesh->GetElementMaterial();
	data.vecMtrlID.resize(pMesh->GetPolygonCount());
	for (int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		pMaterial->GetIndexArray().GetAt(i);

		switch( pMaterial->GetMappingMode())
		{
		case FbxLayerElement::eAllSame:
			data.vecMtrlID[i] = pMaterial->GetIndexArray().GetAt(0);
			break;
		case FbxLayerElement::eByPolygon:
			data.vecMtrlID[i] = pMaterial->GetIndexArray().GetAt(i);
			break;
		default:
			data.vecMtrlID[i] = 0;
			printf("missing material information\n");
		break;
		}
	}


	// 정점 인덱스
	data.vecIndex.resize(pMesh->GetPolygonCount());
	for(int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		TriIndex tri;
		tri.i[0] = pMesh->GetPolygonVertex(i, 0);
		tri.i[1] = pMesh->GetPolygonVertex(i, 1);
		tri.i[2] = pMesh->GetPolygonVertex(i, 2);
		data.vecIndex[i] = TriIndexToI4(tri);

		
	}

	// UV 좌표에 대한 인덱스
	if(pMesh->GetElementUVCount())                        
	{
		data.vecTexIndex.resize(pMesh->GetPolygonCount());
		for (int i = 0; i < pMesh->GetPolygonCount(); i++)
		{		
			FbxGeometryElementUV *puv = pMesh->GetElementUV(0);

			TriIndex tri;
			tri.i[0] = puv->GetIndexArray().GetAt(i*3 + 0);
			tri.i[1] = puv->GetIndexArray().GetAt(i*3 + 1);
			tri.i[2] = puv->GetIndexArray().GetAt(i*3 + 2);
			data.vecTexIndex[i] = TriIndexToI4(tri);
		}
	}

	// UV 인덱스에 해당하는 UV 좌표
	if(pMesh->GetElementUVCount())                        
	{
		FbxGeometryElementUV *puv = pMesh->GetElementUV(0);
		data.vecTexUV.resize(puv->GetDirectArray().GetCount());
		for (int i = 0; i < puv->GetDirectArray().GetCount(); ++i)
		{
			FbxVector2 uv = puv->GetDirectArray().GetAt(i);
			data.vecTexUV[i] = FbxUVToI4(TextureUV(uv.mData[0], uv.mData[1]));
		}
	}

	// 각각 버텍스에 본 가중치들을 추가한다.
	int deformerCount = pMesh->GetDeformerCount();
	if (deformerCount > 0)
	{
		
		data.vecSkinInfo.resize(pMesh->GetControlPointsCount());
		for (int i = 0; i < deformerCount; ++i)
		{
			FbxDeformer* pDeformer = pMesh->GetDeformer(i);
			if (pDeformer == nullptr)
				continue;

			if (pDeformer->GetDeformerType() != FbxDeformer::eSkin)
				continue;

			FbxSkin* pSkin = (FbxSkin*)pDeformer;

			int clusterCount = pSkin->GetClusterCount();
			for (int j = 0; j < clusterCount; ++j)
			{
				FbxCluster* pCluster = pSkin->GetCluster(j);
				if (pCluster == nullptr)
					continue;

				FbxNode* pLinkNode = pCluster->GetLink();
				if (pLinkNode == nullptr)
					continue;

				string name = pLinkNode->GetName();

				int boneID = mapBoneNameList[name];

				int associateCtrlPointCount = pCluster->GetControlPointIndicesCount();
				int* pCtrlPointIndices = pCluster->GetControlPointIndices();
				double* pCtrlPointWeights = pCluster->GetControlPointWeights();

				for (int k = 0; k < associateCtrlPointCount; ++k)
				{
					int nIdex = pCtrlPointIndices[k];
					double weight = pCtrlPointWeights[k];
					SkinInfo info;
					info.boneID = boneID;
					info.boneWeight = weight;
					data.vecSkinInfo[nIdex].data.push_back(info);
				}
			}
		}

		// 얻어온 본 가중치들을 가중치순으로 정렬해준다.
		for (unsigned int i = 0; i < data.vecSkinInfo.size(); ++i)
		{
			sort(data.vecSkinInfo[i].data.begin(), data.vecSkinInfo[i].data.end(), [](const SkinInfo& lhs, const SkinInfo& rhs) { return lhs.boneWeight > rhs.boneWeight; });
		}
	}

	mergeMeshTextureUV(data);

	//-------------------------------------------------------------------------------------------------------------------------------------------------
		
	fprintf(fpMesh, "\t\t<position count=\"%d\">\n", data.vecPosition.size());
	for (int i = 0; i < data.vecPosition.size(); ++i)
	{
		fprintf(fpMesh, "\t\t\t<a>%g %g %g</a>\n", data.vecPosition[i].mData[0], data.vecPosition[i].mData[1], data.vecPosition[i].mData[2]);
	}
	fprintf(fpMesh, "\t\t</position>\n");

	if(data.vecNormal.size() > 0)
	{
		fprintf(fpMesh, "\t\t<normal count=\"%d\">\n", data.vecNormal.size());
		for (int i = 0; i < data.vecNormal.size(); ++i)
		{
			fprintf(fpMesh, "\t\t\t<a>%g %g %g</a>\n", data.vecNormal[i].mData[0], data.vecNormal[i].mData[1], data.vecNormal[i].mData[2]);
		}
		fprintf(fpMesh, "\t\t</normal>\n");
	}

		
	if (data.vecUV.size() > 0)
	{
		fprintf(fpMesh, "\t\t<uv count=\"%d\">\n", data.vecUV.size());
		for (unsigned int i = 0; i < data.vecUV.size(); ++i)
		{
			fprintf(fpMesh, "\t\t\t<a>%g %g</a>\n", data.vecUV[i].u, data.vecUV[i].v);
		}
		fprintf(fpMesh, "\t\t</uv>\n");
	}

	if (data.vecSkinInfo.size() > 0)
	{
		fprintf(fpMesh, "\t\t<weight count=\"%d\">\n", data.vecSkinInfo.size());

		for (unsigned int i = 0; i < data.vecSkinInfo.size(); ++i)
		{
			fprintf(fpMesh, "\t\t\t<v>\n");
			for (int j = 0; j < data.vecSkinInfo[i].data.size(); ++j)
			{
				fprintf(fpMesh, "\t\t\t\t<a>%d %g</a>\n", data.vecSkinInfo[i].data[j].boneID, data.vecSkinInfo[i].data[j].boneWeight);
			}
			fprintf(fpMesh, "\t\t\t</v>\n");
		}
		fprintf(fpMesh, "\t\t</weight>\n");		
	}

	map<int, vector<TriIndex>> mapSubMesh;
	for (int i = 0; i < pMesh->GetPolygonCount(); ++i)
	{
		mapSubMesh[data.vecMtrlID[i]].push_back(data.vecIndex[i]);
	}

	int start = 0;
	fprintf(fpMesh, "\t\t<sub count=\"%d\">\n", mapSubMesh.size());
	for (auto& itr : mapSubMesh)
	{			
		fprintf(fpMesh, "\t\t\t<a id=\"%d\" start=\"%d\" count=\"%d\"/>\n", itr.first, start, itr.second.size());
		start += itr.second.size();
	}

	fprintf(fpMesh, "\t\t</sub>\n");

	fprintf(fpMesh, "\t\t<index count=\"%d\">\n",  pMesh->GetPolygonCount());

	for (auto& itr : mapSubMesh)
	{
		const vector<TriIndex>& vecIndex = itr.second;

		for (int i = 0; i < vecIndex.size(); ++i)
		{
			fprintf(fpMesh, "\t\t\t<a>%d %d %d</a>\n", vecIndex[i].i[0], vecIndex[i].i[1], vecIndex[i].i[2]);			
		}
	}

	fprintf(fpMesh, "\t\t</index>\n");
}

void WriteNodeTransform(FbxNode* pNode, FILE* fp)
{
	FbxMatrix matLocal = FbxMatrixToI4(GetLocalTransform(pNode));
	fprintf(fp, "\t\t<localTM>\n");
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[0][0], matLocal.mData[0][1], matLocal.mData[0][2]);
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[1][0], matLocal.mData[1][1], matLocal.mData[1][2]);
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[2][0], matLocal.mData[2][1], matLocal.mData[2][2]);
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[3][0], matLocal.mData[3][1], matLocal.mData[3][2]);
	fprintf(fp, "\t\t</localTM>\n");

	FbxMatrix matWorld = FbxMatrixToI4(GetWorldTransform(pNode));
	fprintf(fp, "\t\t<worldTM>\n");
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[0][0], matWorld.mData[0][1], matWorld.mData[0][2]);
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[1][0], matWorld.mData[1][1], matWorld.mData[1][2]);
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[2][0], matWorld.mData[2][1], matWorld.mData[2][2]);
	fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[3][0], matWorld.mData[3][1], matWorld.mData[3][2]);
	fprintf(fp, "\t\t</worldTM>\n");
}


void WriteNodeStart(FbxNode* pNode, const char* nodeName, const char* nodeParentName, FILE* fp)
{
	fprintf(fp, "\t<node name=\"%s\" parent_name=\"%s\">\n", nodeName, nodeParentName);
}

void WriteNodeEnd(FILE* fp)
{
	fprintf(fp, "\t</node>\n");
}

void WriteNode(FbxNode* pNode, ExportType exportType)
{
	const char* nodeName = pNode->GetName();
	const char* nodeParentName = "undefined";

	if (pNode->GetParent() != nullptr && pNode->GetParent() != pNode->GetScene()->GetRootNode())
	{
		nodeParentName = pNode->GetParent()->GetName();
	}

	// 정보 덤프
	if (pNode->GetNodeAttribute())
	{
		FbxNodeAttribute::EType type = pNode->GetNodeAttribute()->GetAttributeType();
		switch (type)
		{
		case FbxNodeAttribute::eMesh:
			if (exportType == EXPORT_MESH)
			{
				FbxMesh* pMesh = pNode->GetMesh();
				printf("mesh node : %s\n", nodeName);
				WriteNodeStart(pNode, nodeName, nodeParentName, fpMesh);
				WriteNodeTransform(pNode, fpMesh);
				WriteMesh(pMesh, fpMesh);
				WriteNodeEnd(fpMesh);
				
				
			}

			if (exportType == EXPORT_MTRL)
			{
				printf("material count %d\n", pNode->GetMaterialCount());

				WriteNodeStart(pNode, nodeName, nodeParentName, fpMtrl);
				for (int i = 0; i < pNode->GetMaterialCount(); ++i)
				{
					FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);

					fprintf(fpMtrl, "\t\t<sub id=\"%d\">\n", i);
					const FbxProperty propAmbient = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
					if (propAmbient.IsValid())
					{
						fprintf(fpMtrl, "\t\t\t<ambient>%g</ambient>\n", ConvertToGrayscale(propAmbient.Get<FbxDouble3>()));
					}

					const FbxProperty propSpecularFactor = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
					if (propSpecularFactor.IsValid())
					{						
						fprintf(fpMtrl, "\t\t\t<specularLevel>%g</specularLevel>\n", propSpecularFactor.Get<FbxDouble>());
					}
					
					const FbxProperty propShininess = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
					if (propShininess.IsValid())
					{
						fprintf(fpMtrl, "\t\t\t<specularPower>%g</specularPower>\n", propShininess.Get<FbxDouble>());

					}

					FbxProperty propDiffuseMap = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
					if (propDiffuseMap.IsValid())
					{
						int count = propDiffuseMap.GetSrcObjectCount<FbxTexture>();

						FbxTexture* lTexture = propDiffuseMap.GetSrcObject<FbxTexture>();
						if (lTexture != NULL)
						{
							FbxFileTexture* pTex = FbxCast<FbxFileTexture>(lTexture);
							fprintf(fpMtrl, "\t\t\t<diffuseMap>%s</diffuseMap>\n", pTex->GetFileName());
						}
					}

					FbxProperty propSpecularMap = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
					if (propSpecularMap.IsValid())
					{						                        
                        FbxTexture* lTexture = propSpecularMap.GetSrcObject<FbxTexture>(0);
                        if (lTexture != NULL)
						{
							FbxFileTexture* pTex = FbxCast<FbxFileTexture>(lTexture);
							fprintf(fpMtrl, "\t\t\t<specularMap>%s</specularMap>\n", pTex->GetFileName());
                        }
					}

					FbxProperty propNormalMap = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
					if (propNormalMap.IsValid())
					{						                        
                        FbxTexture* lTexture = propNormalMap.GetSrcObject<FbxTexture>(0);
                        if (lTexture != NULL)
						{
							FbxFileTexture* pTex = FbxCast<FbxFileTexture>(lTexture);
							fprintf(fpMtrl, "\t\t\t<normalMap>%s</normalMap>\n", pTex->GetFileName());
                        }
					}
					fprintf(fpMtrl, "\t\t</sub>\n");
				}
				WriteNodeEnd(fpMtrl);
			}

			break;	
		case FbxNodeAttribute::eSkeleton:
			if (exportType == EXPORT_BONE)
			{
				printf("skeleton node : %s\n", nodeName);

				fprintf(fpBone, "\t<node name=\"%s\" parent_name=\"%s\" id=\"%d\">\n", nodeName, nodeParentName, mapBoneNameList[nodeName]);

				FbxAMatrix matLocal;			
				if (pNode->GetParent())
				{
					FbxAMatrix matParent = mapBoneBindPoseWorld[nodeParentName];
					FbxAMatrix matParentInv = matParent.Inverse();
					matLocal = matParentInv*mapBoneBindPoseWorld[nodeName];
				}
				mapBoneBindPoseLocal[nodeName] = matLocal;

				matLocal = FbxMatrixToI4(matLocal);
				fprintf(fpBone, "\t\t<localTM>\n");
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[0][0], matLocal.mData[0][1], matLocal.mData[0][2]);
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[1][0], matLocal.mData[1][1], matLocal.mData[1][2]);
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[2][0], matLocal.mData[2][1], matLocal.mData[2][2]);
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matLocal.mData[3][0], matLocal.mData[3][1], matLocal.mData[3][2]);
				fprintf(fpBone, "\t\t</localTM>\n");

			
				FbxAMatrix matWorld = mapBoneBindPoseWorld[nodeName]; //FbxMatrixToI4(pNode->EvaluateGlobalTransform(time));

				matWorld = FbxMatrixToI4(matWorld);
				fprintf(fpBone, "\t\t<worldTM>\n");
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[0][0], matWorld.mData[0][1], matWorld.mData[0][2]);
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[1][0], matWorld.mData[1][1], matWorld.mData[1][2]);
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[2][0], matWorld.mData[2][1], matWorld.mData[2][2]);
				fprintf(fpBone, "\t\t\t<a>%g %g %g</a>\n", matWorld.mData[3][0], matWorld.mData[3][1], matWorld.mData[3][2]);
				fprintf(fpBone, "\t\t</worldTM>\n");

				fprintf(fpBone, "\t</node>\n");
			}
			break;
		}

		if (exportType == EXPORT_ANI)
		{
			printf("animation node : %s\n", nodeName);

			FbxAnimStack* animStack = pScene->GetSrcObject<FbxAnimStack>();
			FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();
			FbxTime start = timeSpan.GetStart();
			FbxTime stop = timeSpan.GetStop();
			FbxTime duration = timeSpan.GetDuration();
			FbxLongLong totalFrameCount = duration.GetFrameCount();

			int samplingStepFrame = 2;
			FbxTime step;
			step.SetTime(0,0,0,samplingStepFrame);

			FbxVector4 startPos = FbxVector4(0, 0, 0, 0);
			FbxQuaternion startRot = FbxQuaternion(0, 0, 0, 1);
			FbxVector4 startScale = FbxVector4(1, 1, 1, 0);
	
			bool isPosKey = false;
			bool isRotKey = false;
			bool isScaleKey = false;

			for (FbxTime t = start; t < stop + step; t += step)
			{
				FbxAMatrix mat = pNode->EvaluateLocalTransform(t);
				FbxVector4 pos = mat.GetT();
				FbxQuaternion rot = mat.GetQ();
				FbxVector4 scale = mat.GetS();

				if (!isPosKey)
				{
					if (pos.Compare(startPos, 0.001f) != 0)
					{
						isPosKey = true;
					}
				}

				if (!isRotKey)
				{
					if (rot.Compare(startRot, 0.000001f) != 0)
					{
						isRotKey = true;
					}
				}

				if (!isScaleKey)
				{
					if (scale.Compare(startScale, 0.001f) != 0)
					{
						isScaleKey = true;
					}
				}

				if (isPosKey && isRotKey && isScaleKey)
					break;
			}

			if (isPosKey || isRotKey || isScaleKey)
			{
				WriteNodeStart(pNode, nodeName, nodeParentName, fpAni);

				if (isPosKey)
				{
					vector<int>			vecKeyFrame;
					vector<FbxVector4>	vecKeyPos;

					vecKeyFrame.reserve(totalFrameCount/samplingStepFrame);
					vecKeyPos.reserve(totalFrameCount/samplingStepFrame);	

					FbxVector4 prevPos;
					bool prevEqual = false;
					int frameCount = 0;
					for (FbxTime t = start; t < stop + step; t += step, frameCount += samplingStepFrame)
					{
						FbxAMatrix mat = pNode->EvaluateLocalTransform(t);
						FbxVector4 pos = mat.GetT();

						if (vecKeyPos.size() != 0)
						{
							if (pos.Compare(prevPos, 0.001f) == 0)
							{
								prevEqual = true;
								continue;
							}

							if (prevEqual)	// 이전 키값이 같은 상태가 유지되었으면
							{
								prevEqual = false;

								// 이전 키값을 한번 더 넣어줘서 보간에 의한 의도되지 않은 에니메이션이 이루어지는 것을 막는다.
								vecKeyFrame.push_back(frameCount - samplingStepFrame);
								vecKeyPos.push_back(prevPos);
							}
						}

						prevPos = pos;

						vecKeyFrame.push_back(frameCount);
						vecKeyPos.push_back(pos);
					}

					size_t numKeyPos = vecKeyPos.size();
					fprintf(fpAni, "\t\t<posKey count=\"%d\">\n", numKeyPos);
					for (size_t i = 0; i < numKeyPos; ++i)
					{
						FbxVector4 p = FbxVectorToI4(vecKeyPos[i]);
						fprintf(fpAni, "\t\t\t<a frame=\"%d\">%g %g %g</a>\n", vecKeyFrame[i], p.mData[0], p.mData[1], p.mData[2]);
					}
					fprintf(fpAni, "\t\t</posKey>\n");
				}
				
				if (isRotKey)
				{
					vector<int>				vecKeyFrame;
					vector<FbxQuaternion>	vecKeyRot;				

					vecKeyFrame.reserve(totalFrameCount/samplingStepFrame);
					vecKeyRot.reserve(totalFrameCount/samplingStepFrame);				

					FbxQuaternion prevRot;
					int frameCount = 0;
					bool prevIdentity = false;
					for (FbxTime t = start; t < stop + step; t += step, frameCount += samplingStepFrame)
					{
						FbxAMatrix mat = pNode->EvaluateLocalTransform(t);
						FbxQuaternion rot = mat.GetQ();

						if (vecKeyRot.size() != 0)
						{
							if (rot.Compare(prevRot, 0.0001f) == 0)
							{
								prevIdentity = true;
								continue;
							}

							if (prevIdentity)	// 이전 키값이 같은 상태가 유지되었으면
							{
								prevIdentity = false;

								// 이전 키값을 한번 더 넣어줘서 보간에 의한 의도되지 않은 에니메이션이 이루어지는 것을 막는다.
								vecKeyFrame.push_back(frameCount - samplingStepFrame);
								vecKeyRot.push_back(rot);
							}
						}

						prevRot = rot;

						vecKeyFrame.push_back(frameCount);
						vecKeyRot.push_back(rot);
					}

					size_t numKeyRot = vecKeyRot.size();
					fprintf(fpAni, "\t\t<rotKey count=\"%d\">\n", numKeyRot);
					for (size_t i = 0; i < numKeyRot; ++i)
					{
						FbxQuaternion q = FbxQuatToI4(vecKeyRot[i]);
						fprintf(fpAni, "\t\t\t<a frame=\"%d\">%g %g %g %g</a>\n",  vecKeyFrame[i], q.mData[0], q.mData[1], q.mData[2], q.mData[3]);
					}

					fprintf(fpAni, "\t\t</rotKey>\n");
				}
				
				if (isScaleKey)
				{
					vector<int>			vecKeyFrame;
					vector<FbxVector4>	vecKeyScale;

					vecKeyFrame.reserve(totalFrameCount/samplingStepFrame);
					vecKeyScale.reserve(totalFrameCount/samplingStepFrame);	

					FbxVector4 prevScale;
					bool prevEqual = false;
					int frameCount = 0;
					for (FbxTime t = start; t < stop + step; t += step, frameCount += samplingStepFrame)
					{
						FbxAMatrix mat = pNode->EvaluateLocalTransform(t);
						FbxVector4 scale = mat.GetS();

						if (vecKeyScale.size() != 0)
						{
							if (scale.Compare(prevScale, 0.001f) == 0)
							{
								prevEqual = true;
								continue;
							}

							if (prevEqual)	// 이전 키값이 같은 상태가 유지되었으면
							{
								prevEqual = false;

								// 이전 키값을 한번 더 넣어줘서 보간에 의한 의도되지 않은 에니메이션이 이루어지는 것을 막는다.
								vecKeyFrame.push_back(frameCount - samplingStepFrame);
								vecKeyScale.push_back(prevScale);
							}
						}

						prevScale = scale;

						vecKeyFrame.push_back(frameCount);
						vecKeyScale.push_back(scale);
					}

					size_t numKeyScale = vecKeyScale.size();
					fprintf(fpAni, "\t\t<scaleKey count=\"%d\">\n", numKeyScale);
					for (size_t i = 0; i < numKeyScale; ++i)
					{
						FbxVector4 p = FbxScaleToI4(vecKeyScale[i]);
						fprintf(fpAni, "\t\t\t<a frame=\"%d\">%g %g %g</a>\n", vecKeyFrame[i], p.mData[0], p.mData[1], p.mData[2]);
					}
					fprintf(fpAni, "\t\t</scaleKey>\n");
				}

				WriteNodeEnd(fpAni);
			}			
		}

	}

	for(int j = 0; j < pNode->GetChildCount(); j++)
		WriteNode(pNode->GetChild(j), exportType);


}

// Triangulate all NURBS, patch and mesh under this node recursively.
void TriangulateRecursive(FbxNode* pNode)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometryConverter lConverter(pNode->GetFbxManager());
			lConverter.TriangulateInPlace(pNode);
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		TriangulateRecursive(pNode->GetChild(lChildIndex));
	}
}

void BuildBoneNameList(FbxNode* pNode)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			mapBoneNameList.insert(make_pair(pNode->GetName(), mapBoneNameList.size()));
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh* pMesh = pNode->GetMesh();
			if (pMesh == nullptr)
				return;

			int deformerCount = pMesh->GetDeformerCount();
			for (int i = 0; i < deformerCount; ++i)
			{
				FbxDeformer* pDeformer = pMesh->GetDeformer(i);
				if (pDeformer == nullptr)
					continue;

				if (pDeformer->GetDeformerType() != FbxDeformer::eSkin)
					continue;

				FbxSkin* pSkin = (FbxSkin*)pDeformer;

				int clusterCount = pSkin->GetClusterCount();
				for (int j = 0; j < clusterCount; ++j)
				{
					FbxCluster* pCluster = pSkin->GetCluster(j);
					if (pCluster == nullptr)
						continue;

					FbxNode* pLinkNode = pCluster->GetLink();
					if (pLinkNode == nullptr)
						continue;

					string name = pLinkNode->GetName();

					FbxAMatrix m;
					pCluster->GetTransformLinkMatrix(m);
					mapBoneBindPoseWorld[name] = m*GetGeometricTransform(pLinkNode);
				}
			}
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		BuildBoneNameList(pNode->GetChild(lChildIndex));
	}
}

int main(int argc, char* argv[])
{
	const char* lFilename = "Raven.fbx";	

	FbxManager* lSdkManager = FbxManager::Create();

	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxImporter* lImporter = FbxImporter::Create(lSdkManager,"");

	if(!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	pScene = FbxScene::Create(lSdkManager,"myScene");

	lImporter->Import(pScene);

    SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();

	lImporter->Destroy();
	
	TriangulateRecursive(pScene->GetRootNode());
	BuildBoneNameList(pScene->GetRootNode());
	
	//--------------------------------------------------------------------------
	fpMesh = fopen("raven.mesh.xml", "w");
	if (fpMesh != nullptr)
	{
		fprintf(fpMesh, "<mesh>\n");
	
		fprintf(fpMesh, "\t<version>1.0.0</version>\n");
		FbxNode* lRootNode = pScene->GetRootNode();
		if(lRootNode) {
			for(int i = 0; i < lRootNode->GetChildCount(); i++)
				WriteNode(lRootNode->GetChild(i), EXPORT_MESH);
		}

		fprintf(fpMesh, "</mesh>");
		fclose(fpMesh);
	}

	//--------------------------------------------------------------------------

	fpMtrl = fopen("raven.mtrl.xml", "w");
	if (fpMtrl != nullptr)
	{
		fprintf(fpMtrl, "<material>\n");
		fprintf(fpMtrl, "\t<version>1.0.0</version>\n");
	
		FbxNode* lRootNode = pScene->GetRootNode();
		if(lRootNode) {
			for(int i = 0; i < lRootNode->GetChildCount(); i++)
				WriteNode(lRootNode->GetChild(i), EXPORT_MTRL);
		}

		fprintf(fpMtrl, "</mtrl>");
		fclose(fpMtrl);
	}

	//--------------------------------------------------------------------------

	fpBone = fopen("raven.bone.xml", "w");
	if (fpBone != nullptr)
	{
		fprintf(fpBone, "<bone>\n");
		fprintf(fpMesh, "\t<version>1.0.0</version>\n");
		FbxNode* lRootNode = pScene->GetRootNode();
		if(lRootNode) {
			for(int i = 0; i < lRootNode->GetChildCount(); i++)
				WriteNode(lRootNode->GetChild(i), EXPORT_BONE);
		}
		fprintf(fpBone, "</bone>");
		fclose(fpBone);
	}

	//--------------------------------------------------------------------------

	fpAni = fopen("raven.ani.xml", "w");
	if (fpAni != nullptr)
	{
		fprintf(fpAni, "<ani>\n");
		fprintf(fpAni, "\t<version>1.0.0</version>\n");

		int animStackCount = pScene->GetSrcObjectCount<FbxAnimStack>();
		if (animStackCount > 1)
		{
			printf("only 1 animation stack supported.");
		}

		FbxAnimStack* animStack = pScene->GetSrcObject<FbxAnimStack>(0);
		FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();
		FbxTime start = timeSpan.GetStart();
		FbxTime stop = timeSpan.GetStop();
		FbxTime duration = timeSpan.GetDuration();
		FbxLongLong frame = duration.GetFrameCount();

		fprintf(fpAni, "\t<startFrame>%d</startFrame>\n", 0);
		fprintf(fpAni, "\t<endFrame>%d</endFrame>\n", frame);

		FbxNode* lRootNode = pScene->GetRootNode();
		if(lRootNode) {
			for(int i = 0; i < lRootNode->GetChildCount(); i++)
				WriteNode(lRootNode->GetChild(i), EXPORT_ANI);
		}

		fprintf(fpAni, "</ani>");
		fclose(fpAni);
	}

	lSdkManager->Destroy();

	return 0;
}
