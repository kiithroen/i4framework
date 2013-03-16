// i4converter.cpp : Defines the entry point for the console application.
//

//-----------------------------------------------------------------------------------------------------------------
// 모든 FBX 맵핑, 레퍼런스 모드를 지원하지 않고 일단 이전에 쓰던 포멧형태로 뽑혀진거만 지원한다
// 이유는 특정 방법으로 뽑으면 버텍스 크기가 너무 커지기 때문..
// 추후에 경고를 표시해주던지 다른 방법으로 변환을 하던지 해야겠다.
//-------------------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <fbxsdk.h>
#include <vector>
using namespace std;

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

FbxMatrix GetLocalTransform(FbxNode *pNode)
{
	FbxVector4 t = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 s = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxVector4 r = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxMatrix offset (t,r,s);
	FbxMatrix world = pNode->EvaluateGlobalTransform();
	FbxMatrix transform = world * offset;

	return transform;
}

FbxMatrix GetWorldTransform(FbxNode *pNode)
{
	return pNode->EvaluateGlobalTransform();
}

void WriteMesh(FbxNode* pNode, FILE* fp) 
{
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

	fprintf(fp, "\t\t<vertex count=\"%d\">\n", lControlPointsCount);
	for (int i = 0; i < lControlPointsCount; ++i)
	{
		fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", vecVertex[i].mData[0], vecVertex[i].mData[1], vecVertex[i].mData[2]);
	}
	fprintf(fp, "\t\t</vertex>\n");

	if(pMesh->GetElementNormalCount())
	{
		fprintf(fp, "\t\t<normal count=\"%d\">\n", lControlPointsCount);
		for (int i = 0; i < lControlPointsCount; ++i)
		{
			fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", vecNormal[i].mData[0], vecNormal[i].mData[1], vecNormal[i].mData[2]);
		}
		fprintf(fp, "\t\t</normal>\n");
	}

	fprintf(fp, "\t\t<index count=\"%d\">\n", pMesh->GetPolygonCount());
	for (int i = 0; i < pMesh->GetPolygonCount()*3; i += 3)
	{
		fprintf(fp, "\t\t\t<a>%d %d %d</a>\n", vecIndex[i], vecIndex[i+1], vecIndex[i+2]);
	}
	fprintf(fp, "\t\t</index>\n");

	if (pMesh->GetElementUVCount())
	{
		fprintf(fp, "\t\t<texUV count=\"%d\">\n", vecTexUV.size());
		for (unsigned int i = 0; i < vecTexUV.size(); ++i)
		{
			fprintf(fp, "\t\t\t<a>%g %g</a>\n", vecTexUV[i].mData[0], vecTexUV[i].mData[1]);
		}
		fprintf(fp, "\t\t</texUV>\n");
	}

	fprintf(fp, "\t\t<texIndex count=\"%d\">\n", pMesh->GetPolygonCount());
	for (int i = 0; i < pMesh->GetPolygonCount()*3; i += 3)
	{
		fprintf(fp, "\t\t\t<a>%d %d %d</a>\n", vecTexIndex[i], vecTexIndex[i+1], vecTexIndex[i+2]);
	}
	fprintf(fp, "\t\t</texIndex>\n");
}

void WriteNode(FbxNode* pNode, FILE* fp)
{
	const char* nodeName = pNode->GetName();
	const char* nodeParentName = "undefined";

	printf("%s coverting...\n", pNode->GetName());

	if (pNode->GetParent() != nullptr && pNode->GetParent() != pNode->GetScene()->GetRootNode())
	{
		nodeParentName = pNode->GetParent()->GetName();
	}

	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();
	
	fprintf(fp, "\t<node name=\"%s\" parent_name=\"%s\">\n", nodeName, nodeParentName);

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

	// 정보 덤프
	if (pNode->GetNodeAttribute())
	{
		switch (pNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			WriteMesh(pNode, fp);
			break;	
		case FbxNodeAttribute::eSkeleton:
			break;
		}
	}

	fprintf(fp, "\t</node>\n");

	for(int j = 0; j < pNode->GetChildCount(); j++)
		WriteNode(pNode->GetChild(j), fp);
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

int main(int argc, char* argv[])
{
	const char* lFilename = "Elin.fbx";

	FILE* fp = fopen("out.xml", "w");
	if (fp == nullptr)
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

	FbxScene* lScene = FbxScene::Create(lSdkManager,"myScene");

	lImporter->Import(lScene);

	lImporter->Destroy();

	TriangulateRecursive(lScene->GetRootNode());

	fprintf(fp, "<mesh>\n");
	fprintf(fp, "\t<version>1.0.0</version>\n");

	FbxNode* lRootNode = lScene->GetRootNode();
	if(lRootNode) {
		for(int i = 0; i < lRootNode->GetChildCount(); i++)
			WriteNode(lRootNode->GetChild(i), fp);
	}

	fprintf(fp, "</mesh>");

	lSdkManager->Destroy();

	fclose(fp);

	return 0;
}
