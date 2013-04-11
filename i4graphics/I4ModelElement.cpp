#include "stdafx.h"
#include "I4ModelElement.h"
#include "I4TriangleMesh.h"
#include "I4AnimationController.h"
#include "I4Model.h"
#include "I4ShaderMgr.h"
#include "I4Material.h"
#include "I4VideoDriver.h"
#include "I4Log.h"
#include "I4Camera.h"
#include "I4Renderer.h"
#include "I4Profile.h"
#include "I4Hash.h"

namespace i4graphics
{
	//------------------------- ModelElement ---------------------

	ModelElement::ModelElement(Model* _model, ModelElementInfo* info)
		: model(_model)
		, elementInfo(info)
		, parentElement(nullptr)
		, aniController(nullptr)
	{
		resultTM = elementInfo->localTM;
	}

	ModelElement::~ModelElement()
	{
		delete aniController;
	}

	void ModelElement::registerAni(const char* name, KeyFrameSet* keyFrameSet, float start, float end)
	{
		if (aniController == nullptr)
		{
			aniController = new AnimationController;
		}
		
		aniController->addTrack(name, keyFrameSet, start, end);
	}

	void ModelElement::playAni(const char* name)
	{
		if (aniController == nullptr)
			return;

		aniController->playTrack(name);
	}

	bool ModelElement::initialize()
	{
		parentElement = model->findElement(elementInfo->parentName.c_str());

		return true;
	}

	void ModelElement::animate(float dt, const Matrix4x4& parentTM)
	{
		if (aniController == nullptr)
		{
			resultTM = elementInfo->localTM*parentTM;
		}
		else
		{
			aniController->animate(dt);
			resultTM = aniController->getAnimationTM();
			resultTM *= parentTM;
		}
	}

	//------------------------- ModelBone ---------------------

	ModelBone::ModelBone(Model* model, ModelElementInfo* info)
	: ModelElement(model, info)
	{
	}

	ModelBone::~ModelBone()
	{
	}

	bool ModelBone::initialize()
	{
		ModelElement::initialize();

		elementInfo->worldTM.extractInversePrimitive(worldInverseTM);

		return true;
	}

	void ModelBone::commitToRenderer(Renderer* renderer, const Matrix4x4& parentTM)
	{
		const Matrix4x4 worldTM = resultTM*parentTM;

		DebugBox box;
		box.aabb = AABB(Vector3(-1, -1, -1), Vector3(1, 1, 1)).transform(worldTM);
		box.color = Vector4(1, 0, 0, 1);
		renderer->commit(box);
	}

	//------------------------- ModelMesh -------------------------

	static Material defaultMaterial;

	ModelMesh::ModelMesh(Model* model, ModelElementInfo* info, TriangleMesh* _mesh)
	: ModelElement(model, info)
	, mesh(_mesh)
	{
		vecMaterial.resize(mesh->subMeshes.size());
		for (unsigned int i = 0; i < vecMaterial.size(); ++i)
		{
			vecMaterial[i] = &defaultMaterial;
		}
	}

	ModelMesh::~ModelMesh()
	{
	}


	void ModelMesh::commitToRenderer(Renderer* renderer, const Matrix4x4& worldTM)
	{
		const Matrix4x4 finalTM = resultTM*worldTM;

		MeshRenderItem item;
		item.mesh = mesh;

		
		// 스킨드메쉬는 정확한 바운드를 GPU에서 에니메이션 하기전에 알수 없으므로 문제가 생길수 있다.
		// 툴에서 수동으로 또는 미리 계산해서 지정하도록 바꾸자.
		item.worldAABB = mesh->localAABB.transform(finalTM);

		item.shadowCaster = model->isShadowCaster();
		item.shadowReceiver = model->isShadowReceiver();

		item.shaderMask = SHADER_MASK_NONE;

		if (mesh->skined && model->getBoneCount() > 0)
		{
			item.shaderMask |= SHADER_MASK_SKINNING;
			item.worldTM = worldTM;
			item.boneCount = elementInfo->boneRefTable.size();
			item.resultTM = resultTM;
			item.boneRefTable = &elementInfo->boneRefTable[0];
			item.skinTMs = model->getSkinTMs();
		}
		else
		{
			item.worldTM = finalTM;
			// 아래 정보들은 사용안함
			item.boneCount = 0;
			item.skinTMs = nullptr;
			item.boneRefTable = nullptr;
			item.resultTM = MATRIX4X4_IDENTITY; 
		}		

		for (unsigned int i = 0; i < mesh->subMeshes.size(); ++i)
		{
			item.subMeshID = i;
			item.material = vecMaterial[i];

			if (item.material->diffuseMap != INVALID_HASHCODE)
			{
				item.shaderMask |= SHADER_MASK_TEX_DIFFUSE;
			}

			if (item.material->specularMap != INVALID_HASHCODE)
			{
				item.shaderMask |= SHADER_MASK_TEX_SPECULAR;
			}

			if (item.material->normalMap != INVALID_HASHCODE)
			{
				item.shaderMask |= SHADER_MASK_TEX_NORMAL;
			}

			renderer->commit(item);
		}
	}
}