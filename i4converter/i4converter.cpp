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
#include <string>
#include <algorithm>
using namespace std;

FbxScene* pScene = nullptr;

FILE* fpMesh = nullptr;
FILE* fpMtrl = nullptr;
FILE* fpBone = nullptr;
FILE* fpAni = nullptr;

vector<string> vecBoneNameList;

FbxVector2 FbxUVToI4(const FbxVector2& v)
{
	return FbxVector2(v.mData[0], 1.0f - v.mData[1]);
}

FbxVector4 FbxVectorToI4(const FbxVector4& v)
{
	return FbxVector4(v.mData[0], v.mData[2], -v.mData[1]);
}

FbxMatrix FbxMatrixToI4(const FbxMatrix& m)
{
	FbxMatrix ret;
	ret.SetRow(0, FbxVectorToI4(m.GetRow(0)));
	ret.SetRow(1, FbxVectorToI4(m.GetRow(2)));
	ret.SetRow(2, -FbxVectorToI4(m.GetRow(1)));
	ret.SetRow(3, FbxVectorToI4(m.GetRow(3)));

	return ret;
}

FbxQuaternion FbxQuatToI4(const FbxQuaternion& q)
{
	return FbxQuaternion(-q.mData[0], -q.mData[2], q.mData[1], -q.mData[3]);
}

FbxMatrix GetLocalTransform(FbxNode *pNode)
{
	/*
	FbxVector4 t = pNode->LclTranslation;
	FbxVector4 s = pNode->LclScaling;
	FbxVector4 r = pNode->LclRotation;

	return FbxMatrix(t, r, s);
	*/
	return pNode->EvaluateLocalTransform();
}

FbxMatrix GetWorldTransform(FbxNode *pNode)
{
	return pNode->EvaluateGlobalTransform();
}

void WriteMesh(FbxNode* pNode, FILE* fpMesh) 
{
	printf("%s write mesh ...\n", pNode->GetName());

	FbxMesh* pMesh = pNode->GetMesh();
	if (pMesh == nullptr)
		return;

	vector<FbxVector4> vecVertex;
	vector<FbxVector4> vecNormal;
	vector<int> vecTexIndex;

	int lControlPointsCount = pMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();

	// 참조하고 있는 모든 정점
	for (int i = 0; i < lControlPointsCount; i++)
	{
		vecVertex.push_back(FbxVectorToI4(lControlPoints[i]));
	}

	if (pMesh->GetElementNormalCount())                        
	{
		// 정점이 참조하고 있는 노말
		vecNormal.resize(lControlPointsCount);
		for(int i = 0; i < pMesh->GetPolygonCount(); i++)
		{
			for(int j = 0; j < 3; j++) 
			{
				FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(0);

				// 버텍스 인덱스로부터 정점 인덱스를 가져온다.
				int idx = pMesh->GetPolygonVertex(i, j);

				// 그리고 정점인덱스에 해당하는 노말값을 가져와서 집어넣는다.
				vecNormal[idx] = FbxVectorToI4(leNormals->GetDirectArray().GetAt(i*3 + j));
			}
		}
	}

	// 정점 인덱스
	vector<int> vecIndex;
	for(int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		for(int j = 0; j < 3; j++) 
		{
			int idx = pMesh->GetPolygonVertex(i, j);

			vecIndex.push_back(idx);
		}
	}

	// UV 좌표에 대한 인덱스
	if(pMesh->GetElementUVCount())                        
	{
		for (int i = 0; i < pMesh->GetPolygonCount(); i++)
		{		
			FbxGeometryElementUV *puv = pMesh->GetElementUV(0);

			for (int j = 0; j < 3; ++j)
			{
				int index = puv->GetIndexArray().GetAt(i*3 + j);
				vecTexIndex.push_back(index);
			}
		}
	}

	// UV 인덱스에 해당하는 UV 좌표
	vector<FbxVector2> vecTexUV;
	if(pMesh->GetElementUVCount())                        
	{
		FbxGeometryElementUV *puv = pMesh->GetElementUV(0);

		for (int i = 0; i < puv->GetDirectArray().GetCount(); ++i)
		{
			vecTexUV.push_back(FbxUVToI4(puv->GetDirectArray().GetAt(i)));
		}
	}

	fprintf(fpMesh, "\t\t<vertex count=\"%d\">\n", lControlPointsCount);
	for (int i = 0; i < lControlPointsCount; ++i)
	{
		fprintf(fpMesh, "\t\t\t<a>%g %g %g</a>\n", vecVertex[i].mData[0], vecVertex[i].mData[1], vecVertex[i].mData[2]);
	}
	fprintf(fpMesh, "\t\t</vertex>\n");

	if(pMesh->GetElementNormalCount())
	{
		fprintf(fpMesh, "\t\t<normal count=\"%d\">\n", lControlPointsCount);
		for (int i = 0; i < lControlPointsCount; ++i)
		{
			fprintf(fpMesh, "\t\t\t<a>%g %g %g</a>\n", vecNormal[i].mData[0], vecNormal[i].mData[1], vecNormal[i].mData[2]);
		}
		fprintf(fpMesh, "\t\t</normal>\n");
	}

	fprintf(fpMesh, "\t\t<index count=\"%d\">\n", pMesh->GetPolygonCount());
	for (int i = 0; i < pMesh->GetPolygonCount()*3; i += 3)
	{
		fprintf(fpMesh, "\t\t\t<a>%d %d %d</a>\n", vecIndex[i], vecIndex[i+1], vecIndex[i+2]);
	}
	fprintf(fpMesh, "\t\t</index>\n");

	if (pMesh->GetElementUVCount())
	{
		fprintf(fpMesh, "\t\t<texUV count=\"%d\">\n", vecTexUV.size());
		for (unsigned int i = 0; i < vecTexUV.size(); ++i)
		{
			fprintf(fpMesh, "\t\t\t<a>%g %g</a>\n", vecTexUV[i].mData[0], vecTexUV[i].mData[1]);
		}
		fprintf(fpMesh, "\t\t</texUV>\n");
	}

	fprintf(fpMesh, "\t\t<texIndex count=\"%d\">\n", pMesh->GetPolygonCount());
	for (int i = 0; i < pMesh->GetPolygonCount()*3; i += 3)
	{
		fprintf(fpMesh, "\t\t\t<a>%d %d %d</a>\n", vecTexIndex[i], vecTexIndex[i+1], vecTexIndex[i+2]);
	}
	fprintf(fpMesh, "\t\t</texIndex>\n");

	struct SkinInfo
	{
		int boneID;
		float boneWeight;
	};

	struct SkinData
	{
		vector<SkinInfo>	data;
	};

	vector<SkinData>	vecSkinInfo;
	vecSkinInfo.resize(pMesh->GetControlPointsCount());
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

			int boneID = -1;
			for (int idx = 0; idx < vecBoneNameList.size(); ++idx)
			{
				if (strcmp(vecBoneNameList[idx].c_str(), name.c_str()) == 0)
				{
					boneID = idx;
					break;
				}
			}

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
				vecSkinInfo[nIdex].data.push_back(info);
			}
		}
	}

	if (vecSkinInfo.size() > 0)
	{
		fprintf(fpMesh, "\t\t<weight count=\"%d\">\n", vecSkinInfo.size());

		for (unsigned int i = 0; i < vecSkinInfo.size(); ++i)
		{
			sort(vecSkinInfo[i].data.begin(), vecSkinInfo[i].data.end(), [](const SkinInfo& lhs, const SkinInfo& rhs) { return lhs.boneWeight > rhs.boneWeight; });
			fprintf(fpMesh, "\t\t\t<v>\n");
			for (int j = 0; j < vecSkinInfo[i].data.size(); ++j)
			{
				fprintf(fpMesh, "\t\t\t\t<a>%d %g</a>\n", vecSkinInfo[i].data[j].boneID, vecSkinInfo[i].data[j].boneWeight);
			}
			fprintf(fpMesh, "\t\t\t</v>\n");
		}
		fprintf(fpMesh, "\t\t</weight>\n");
	}
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

void WriteNode(FbxNode* pNode)
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
			WriteNodeStart(pNode, nodeName, nodeParentName, fpMesh);
			WriteNodeTransform(pNode, fpMesh);
			WriteMesh(pNode, fpMesh);
			WriteNodeEnd(fpMesh);
			break;	
		case FbxNodeAttribute::eSkeleton:
			WriteNodeStart(pNode, nodeName, nodeParentName, fpBone);
			WriteNodeTransform(pNode, fpBone);
			WriteNodeEnd(fpBone);
			break;
		}

		if (type == FbxNodeAttribute::eSkeleton)
		{
			FbxAnimStack* animStack = pScene->GetSrcObject<FbxAnimStack>();
			FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();
			FbxTime start = timeSpan.GetStart();
			FbxTime stop = timeSpan.GetStop();
			FbxTime duration = timeSpan.GetDuration();
			FbxLongLong totalFrameCount = duration.GetFrameCount();

			int samplingStepFrame = 2;
			FbxTime step;
			step.SetTime(0,0,0,samplingStepFrame);

			bool isPosKey = false;
			bool isRotKey = false;

			FbxAMatrix startMat = pNode->EvaluateLocalTransform();
			FbxVector4 startPos = startMat.GetT();
			FbxQuaternion startRot = startMat.GetQ();

			for (FbxTime t = start; t < stop + step; t += step)
			{
				FbxAMatrix mat = pNode->EvaluateLocalTransform(t);
				FbxVector4 pos = mat.GetT();
				FbxQuaternion rot = mat.GetQ();

				if (!isPosKey)
				{
					if (pos != startPos)
					{
						isPosKey = true;
					}
				}

				if (!isRotKey)
				{
					if (rot != startRot)
					{
						isRotKey = true;
					}
				}

				if (isPosKey && isRotKey)
					break;
			}

			if (isPosKey || isRotKey)
			{
				WriteNodeStart(pNode, nodeName, nodeParentName, fpAni);

				if (isPosKey)
				{
					vector<int>			vecKeyFrame;
					vector<FbxVector4>	vecKeyPos;

					vecKeyFrame.reserve(totalFrameCount/samplingStepFrame);
					vecKeyPos.reserve(totalFrameCount/samplingStepFrame);	

					FbxVector4 prevPos = startPos;
					bool prevEqual = false;
					int frameCount = 0;
					for (FbxTime t = start; t < stop + step; t += step, frameCount += samplingStepFrame)
					{
						FbxAMatrix mat = pNode->EvaluateLocalTransform(t);
						FbxVector4 pos = mat.GetT();

						if (vecKeyPos.size() != 0)
						{
							if (pos == prevPos)
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

					FbxQuaternion prevRot = startRot;
					FbxQuaternion accumRot;
					int frameCount = 0;
					bool prevIdentity = false;
					FbxQuaternion qIndentity = FbxQuaternion(0, 0, 0, 1);
					for (FbxTime t = start; t < stop + step; t += step, frameCount += samplingStepFrame)
					{

						FbxAMatrix mat = pNode->EvaluateLocalTransform(t);
						FbxQuaternion quat = mat.GetQ();

						FbxQuaternion invPrevRot = prevRot;
						invPrevRot.Inverse();
						FbxQuaternion deltaQuat = quat*invPrevRot;

						if (vecKeyRot.size() != 0)
						{
							if (deltaQuat == qIndentity)
							{
								prevIdentity = true;
								continue;
							}

							if (prevIdentity)	// 이전 키값이 같은 상태가 유지되었으면
							{
								prevIdentity = false;

								// 이전 키값을 한번 더 넣어줘서 보간에 의한 의도되지 않은 에니메이션이 이루어지는 것을 막는다.
								vecKeyFrame.push_back(frameCount - samplingStepFrame);
								vecKeyRot.push_back(accumRot);
							}
						}
						
						accumRot = deltaQuat*accumRot;
						prevRot = quat;

						vecKeyFrame.push_back(frameCount);
						vecKeyRot.push_back(accumRot);
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
				
				WriteNodeEnd(fpAni);
			}
			
		}

	}

	for(int j = 0; j < pNode->GetChildCount(); j++)
		WriteNode(pNode->GetChild(j));

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
			vecBoneNameList.push_back(pNode->GetName());
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

	fpMesh = fopen("raven.mesh.xml", "w");
	if (fpMesh == nullptr)
		return 0;

	fpMtrl = fopen("raven.mtrl.xml", "w");
	if (fpMtrl == nullptr)
		return 0;

	fpBone = fopen("raven.bone.xml", "w");
	if (fpBone == nullptr)
		return 0;

	fpAni = fopen("raven.ani.xml", "w");
	if (fpAni == nullptr)
		return 0;

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

	lImporter->Destroy();

	TriangulateRecursive(pScene->GetRootNode());
	BuildBoneNameList(pScene->GetRootNode());
	
	fprintf(fpMesh, "<mesh>\n");
	fprintf(fpMesh, "\t<version>1.0.0</version>\n");

	fprintf(fpMtrl, "<material>\n");
	fprintf(fpMtrl, "\t<version>1.0.0</version>\n");

	fprintf(fpBone, "<bone>\n");
	fprintf(fpBone, "\t<version>1.0.0</version>\n");

	fprintf(fpAni, "<ani>\n");
	fprintf(fpAni, "\t<version>1.0.0</version>\n");

	int animStackCount = pScene->GetSrcObjectCount<FbxAnimStack>();
	if (animStackCount > 1)
	{
		printf("only 1 animation stack supported.");
		return 0;
	}

	FbxAnimStack* animStack = pScene->GetSrcObject<FbxAnimStack>();
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
			WriteNode(lRootNode->GetChild(i));
	}

	fprintf(fpMesh, "</mesh>");

	fprintf(fpMtrl, "</mtrl>");

	fprintf(fpBone, "</bone>");

	fprintf(fpAni, "</ani>");

	lSdkManager->Destroy();

	fclose(fpBone);
	fclose(fpMesh);

	return 0;
}
