#pragma once

#include <fbxsdk.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <set>
using namespace std;

enum ExportType
{
	EXPORT_MESH = 0,
	EXPORT_MTRL,
	EXPORT_BONE,
	EXPORT_ANI,
};

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
	vector<SkinData>		vecI4SkinInfo;
	vector<int>				vecMtrlID;
};


class FbxConverter
{
public:
	FbxConverter(void);
	~FbxConverter(void);

	void Initialize();
	void Finalize();

	void Begin(const char* srcFileName, const char* destName);
	void WriteMeshes();
	void WriteMaterials();
	void WriteBones();
	void WriteAnimations();
	void End();

private:	
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
		return (float)(col.mData[0]*0.3f + col.mData[1]*0.59f + col.mData[2]*0.11f);
	}

	TextureUV FbxUVToI4(const TextureUV& v)
	{
		return TextureUV(v.u, 1.0f - v.v);
	}

	FbxVector4 FbxVectorToI4(const FbxVector4& v)
	{
		return FbxVector4(v.mData[0], v.mData[1], -v.mData[2]);
	}

	FbxVector4 FbxNormalToI4(const FbxVector4& v)
	{
		if (isFlipNormalZ)
		{
			return FbxVector4(v.mData[0], v.mData[2], v.mData[1]);
		}
		else
		{
			return FbxVector4(v.mData[0], v.mData[1], -v.mData[2]);
		}
	}

	FbxVector4 FbxScaleToI4(const FbxVector4& v)
	{
		return FbxVector4(v.mData[0], v.mData[2], v.mData[1]);
	}


	TriIndex TriIndexToI4(const TriIndex& i)
	{
		return TriIndex(i.i[0], i.i[2], i.i[1]);
	}

	FbxAMatrix FbxMatrixToI4(const FbxAMatrix& m)
	{
		FbxAMatrix ret;
		ret.SetRow(0, FbxVectorToI4(m.GetRow(0)));
		ret.SetRow(1, FbxVectorToI4(m.GetRow(1)));
		ret.SetRow(2, -FbxVectorToI4(m.GetRow(2)));
		ret.SetRow(3, FbxVectorToI4(m.GetRow(3)));
		return ret;
	}

	FbxQuaternion FbxQuatToI4(const FbxQuaternion& q)
	{
		return FbxQuaternion(-q.mData[0], -q.mData[1], q.mData[2], q.mData[3]);
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
		
	void splitVertexDifferentI4TextureUV(MeshData& out);
	void WriteMesh(FbxMesh* pMesh, FILE* fpMesh);
	void WriteNodeTransform(FbxNode* pNode, FILE* fp);
	void WriteNodeStart(FbxNode* pNode, const char* nodeName, const char* nodeParentName, FILE* fp);
	void WriteNodeEnd(FILE* fp);
	void WriteNode(FbxNode* pNode, ExportType ExportType);
	void TriangulateRecursive(FbxNode* pNode);
	void BuildBoneNameList(FbxNode* pNode);

private:
	FbxManager* lSdkManager;
	FbxScene* pScene;

	FbxAxisSystem SceneAxisSystem;

	// 도저히 이유는 모르겠고... 에니메이션이 적용된 모델은은 노말이 뒤집혀있다.
	// 1. 스키닝한다고 바인딩포즈를 곱해주는데 이부분이 잘못되서 여기서 중복으로 곱해지거나.. 
	// 2. 셰이더에서 스키닝시에 노말을 잘못 트랜스폼시키거나.. 
	// 둘중에 하나일거 같은데 일단 에니메이션이 적용되어있으면 강제로 노말을 뒤집어주도록 했다.
	bool isFlipNormalZ;	

	FILE* fpMesh;
	FILE* fpMtrl;
	FILE* fpBone;
	FILE* fpAni;

	string destBaseName;

	map<string, int> mapBoneNameList;
	map<string, FbxAMatrix> mapBoneBindPoseWorld;
	map<string, FbxAMatrix> mapBoneBindPoseLocal;
};

