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

			// ���� ��ġŰ���� ������ ������ġ��
			if (animationTM._41 == 0.0f && animationTM._42 == 0.0f && animationTM._43 == 0.0f)
			{
				resultTM._41 = elementInfo->localTM._41;
				resultTM._42 = elementInfo->localTM._42;
				resultTM._43 = elementInfo->localTM._43;
			}
			else	// ������ ��ġŰ������ ����(�������������� ��ġ���� �ι� ����ȴ�)
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

		elementInfo->worldTM.extractInversePrimitive(worldInverseTM);

		return true;
	}

	void I4ModelBone::commitToRenderer(I4Renderer* renderer, const I4Matrix4x4& parentTM)
	{
	}

	//------------------------- I4ModelMesh -------------------------

	static I4Material defaultMaterial;

	I4ModelMesh::I4ModelMesh(I4Model* model, I4ModelElementInfo* info, I4TriangleMesh* _mesh)
	: I4ModelElement(model, info)
	, mesh(_mesh)
	, material(&defaultMaterial)
	{
	}

	I4ModelMesh::~I4ModelMesh()
	{
	}


	void I4ModelMesh::commitToRenderer(I4Renderer* renderer, const I4Matrix4x4& worldTM)
	{
		const I4Matrix4x4 finalTM = resultTM*worldTM;

		I4MeshRenderItem item;
		item.mesh = mesh;
		item.material = material;

		// ��Ų��޽��� ��Ȯ�� �ٿ�带 GPU���� ���ϸ��̼� �ϱ����� �˼� �����Ƿ� ������ ����� �ִ�.
		// ������ �������� �Ǵ� �̸� ����ؼ� �����ϵ��� �ٲ���.
		item.worldAABB = mesh->localAABB.transform(finalTM);

		item.shadowCaster = model->isShadowCaster();
		item.shadowReceiver = model->isShadowReceiver();

		item.shaderMask = I4SHADER_MASK_NONE;

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

		if (mesh->skined)
		{
			item.shaderMask |= I4SHADER_MASK_SKINNING;
			item.worldTM = worldTM;
			item.boneCount = model->getBoneCount();
			item.resultTM = resultTM;
			item.skinTMs = model->getSkinTMs();
		}
		else
		{
			item.worldTM = finalTM;
			// �Ʒ� �������� ������
			//item.boneCount = 0;
			//item.skinTMs = nullptr;
			//item.resultTM = I4MATRIX4X4_IDENTITY; 
		}		

		renderer->commit(item);
	}
}