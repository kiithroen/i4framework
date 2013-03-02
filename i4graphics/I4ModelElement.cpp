#include "stdafx.h"
#include "I4ModelElement.h"
#include "I4Mesh.h"
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
	//------------------------- I4ModelElement ---------------------

	I4ModelElement::I4ModelElement(I4Model* _model, I4ModelElementInfo* info)
		: model(_model)
		, elementInfo(info)
		, parentElement(nullptr)
		, aniController(nullptr)
	{
		resultTM = elementInfo->localTM;
	}

	I4ModelElement::~I4ModelElement()
	{
		delete aniController;
	}

	void I4ModelElement::registerAni(const char* name, I4KeyFrameSet* keyFrameSet)
	{
		if (aniController == nullptr)
		{
			aniController = new I4AnimationController;
		}
		
		aniController->addTrack(name, keyFrameSet);
	}

	void I4ModelElement::playAni(const char* name)
	{
		if (aniController == nullptr)
			return;

		aniController->playTrack(name);
	}

	bool I4ModelElement::initialize()
	{
		parentElement = model->findElement(elementInfo->parentName.c_str());

		return true;
	}

	void I4ModelElement::animate(float dt, const I4Matrix4x4& parentTM)
	{
		if (aniController == nullptr)
		{
			resultTM = elementInfo->localTM*parentTM;
		}
		else
		{
			aniController->animate(dt);
			const I4Matrix4x4& animationTM = aniController->getAnimationTM();

			resultTM = elementInfo->localTM*animationTM;

			// 만약 위치키값이 없으면 로컬위치로
			if (animationTM._41 == 0.0f && animationTM._42 == 0.0f && animationTM._43 == 0.0f)
			{
				resultTM._41 = elementInfo->localTM._41;
				resultTM._42 = elementInfo->localTM._42;
				resultTM._43 = elementInfo->localTM._43;
			}
			else	// 있으면 위치키값으로 대입(대입하지않으면 위치값이 두번 적용된다)
			{
				resultTM._41 = animationTM._41;
				resultTM._42 = animationTM._42;
				resultTM._43 = animationTM._43;
			}

			resultTM *= parentTM;
		}
	}

	//------------------------- I4ModelBone ---------------------

	I4ModelBone::I4ModelBone(I4Model* model, I4ModelElementInfo* info)
	: I4ModelElement(model, info)
	{
	}

	I4ModelBone::~I4ModelBone()
	{
	}

	bool I4ModelBone::initialize()
	{
		I4ModelElement::initialize();

		elementInfo->worldTM.extractInversePrimitive(worldInverseTM);;

		return true;
	}

	void I4ModelBone::render(I4Renderer* renderer, const I4Matrix4x4& parentTM)
	{
	}

	//------------------------- I4ModelMesh -------------------------

	static I4Material defaultMaterial;

	I4ModelMesh::I4ModelMesh(I4Model* model, I4ModelElementInfo* info, I4Mesh* _mesh)
	: I4ModelElement(model, info)
	, mesh(_mesh)
	, material(&defaultMaterial)
	{
	}

	I4ModelMesh::~I4ModelMesh()
	{
	}

	//------------------------- ModelRigidMesh -------------------------

	ModelRigidMesh::ModelRigidMesh(I4Model* model, I4ModelElementInfo* info, I4Mesh* _mesh)
	: I4ModelMesh(model, info, _mesh)
	{
	}

	ModelRigidMesh::~ModelRigidMesh()
	{
		
	}

	void ModelRigidMesh::render(I4Renderer* renderer, const I4Matrix4x4& parentTM)
	{
		const I4Matrix4x4 matWorld = resultTM*parentTM;
		I4MeshRenderItem item;
		item.shaderMask = I4SHADER_MASK_NONE;
		item.material = material;
		item.mesh = mesh;
		item.shadowCaster = model->isShadowCaster();

		if (item.material->diffuseMap != I4INVALID_HASHCODE)
		{
			item.shaderMask |= I4SHADER_MASK_TEX_DIFFUSE;
		}

		if (item.material->specularMap != I4INVALID_HASHCODE)
		{
			item.shaderMask |= I4SHADER_MASK_TEX_SPECULAR;
		}

		if (item.material->normalMap != I4INVALID_HASHCODE)
		{
			item.shaderMask |= I4SHADER_MASK_TEX_NORMAL;
		}

		item.boneCount = 0;
		item.worldAABB = mesh->localAABB.transform(matWorld);
		item.worldTM = matWorld;
		item.matrixPalette = nullptr;
		renderer->commitToScene(item);
	}


	//------------------------- ModelSkinedMeshGPU -------------------------

	ModelSkinedMeshGPU::ModelSkinedMeshGPU(I4Model* model, I4ModelElementInfo* info, I4Mesh* _mesh)
	: I4ModelMesh(model, info, _mesh)
	{
	}

	ModelSkinedMeshGPU::~ModelSkinedMeshGPU()
	{

	}

	bool ModelSkinedMeshGPU::initialize()
	{
		I4ModelMesh::initialize();

		matrixPalette.resize(model->getBoneCount());

		return true;
	}

	void ModelSkinedMeshGPU::animate(float dt, const I4Matrix4x4& parentTM)
	{
		I4ModelMesh::animate(dt, parentTM);

		assert(model->getBoneCount() <= matrixPalette.size());
		for (unsigned int i = 0; i < model->getBoneCount(); ++i)
		{
			matrixPalette[i] = resultTM*model->getSkinTM(i);
		}
	}

	void ModelSkinedMeshGPU::render(I4Renderer* renderer, const I4Matrix4x4& parentTM)
	{
		// 스킨드메쉬는 정확한 바운드를 GPU에서 에니메이션 하기전에 알수 없으므로 문제가 생길수 있다.
		// 툴에서 수동으로 또는 미리 계산해서 지정하도록 바꾸자.
		I4MeshRenderItem item;
		item.mesh = mesh;
		item.material = material;
		item.shaderMask = I4SHADER_MASK_SKINNING;
		item.shadowCaster = model->isShadowCaster();

		if (item.material->diffuseMap != I4INVALID_HASHCODE)
		{
			item.shaderMask |= I4SHADER_MASK_TEX_DIFFUSE;
		}

		if (item.material->specularMap != I4INVALID_HASHCODE)
		{
			item.shaderMask |= I4SHADER_MASK_TEX_SPECULAR;
		}

		if (item.material->normalMap != I4INVALID_HASHCODE)
		{
			item.shaderMask |= I4SHADER_MASK_TEX_NORMAL;
		}

		item.boneCount = model->getBoneCount();
		item.worldAABB = mesh->localAABB.transform(resultTM*parentTM);
		item.worldTM = parentTM;
		item.matrixPalette = &matrixPalette[0];
		renderer->commitToScene(item);
	}

}