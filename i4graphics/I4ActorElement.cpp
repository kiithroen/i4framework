#include "I4ActorElement.h"
#include "I4Mesh.h"
#include "I4AnimationController.h"
#include "I4Actor.h"
#include "I4ShaderMgr.h"
#include "I4Material.h"
#include "I4VideoDriver.h"
#include "I4Log.h"
#include "I4Camera.h"
#include "I4DefferedRenderer.h"
#include "I4Profile.h"

namespace i4graphics
{
	//------------------------- I4ActorElement ---------------------

	I4ActorElement::I4ActorElement(I4Actor* _actor, I4ActorElementInfo* info)
		: actor(_actor)
		, elementInfo(info)
		, parentElement(nullptr)
		, aniController(nullptr)
	{
	}

	I4ActorElement::~I4ActorElement()
	{
		delete aniController;
	}

	void I4ActorElement::registerAni(const char* name, I4KeyFrameSet* keyFrameSet)
	{
		if (aniController == nullptr)
		{
			aniController = new I4AnimationController;
		}
		
		aniController->addTrack(name, keyFrameSet);
	}

	void I4ActorElement::playAni(const char* name)
	{
		if (aniController == nullptr)
			return;

		aniController->playTrack(name);
	}

	bool I4ActorElement::initialize()
	{
		resultTM = elementInfo->localTM;

		parentElement = actor->findElement(elementInfo->parentName.c_str());

		return true;
	}

	void I4ActorElement::animate(float deltaSec, const I4Matrix4x4& parentTM)
	{
		if (aniController == nullptr)
		{
			resultTM = elementInfo->localTM*parentTM;
		}
		else
		{
			aniController->animate(deltaSec);
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

	//------------------------- I4ActorBone ---------------------

	I4ActorBone::I4ActorBone(I4Actor* actor, I4ActorElementInfo* info)
	: I4ActorElement(actor, info)
	{
	}

	I4ActorBone::~I4ActorBone()
	{
	}

	bool I4ActorBone::initialize()
	{
		I4ActorElement::initialize();

		elementInfo->worldTM.extractInversePrimitive(worldInverseTM);;

		return true;
	}

	void I4ActorBone::render(I4DefferedRenderer* renderer, const I4Matrix4x4& parentTM)
	{
	}

	//------------------------- I4ActorMesh -------------------------

	I4ActorMesh::I4ActorMesh(I4Actor* actor, I4ActorElementInfo* info, I4Mesh* _mesh)
	: I4ActorElement(actor, info)
	, mesh(_mesh)
	{
	}

	I4ActorMesh::~I4ActorMesh()
	{
	}

	//------------------------- ActorRigidMesh -------------------------

	ActorRigidMesh::ActorRigidMesh(I4Actor* actor, I4ActorElementInfo* info, I4Mesh* _mesh)
	: I4ActorMesh(actor, info, _mesh)
	{
	}

	ActorRigidMesh::~ActorRigidMesh()
	{
		
	}

	void ActorRigidMesh::render(I4DefferedRenderer* renderer, const I4Matrix4x4& parentTM)
	{
		const I4Matrix4x4 matWorld = resultTM*parentTM;
		I4MeshInstanceRenderItem item;
		item.shaderMask = I4SHADER_MASK_NONE;
		item.mesh = mesh;
		item.boneCount = 0;
		item.worldAABB = mesh->localAABB.transform(matWorld);
		item.worldTM = matWorld;
		item.matrixPalette = nullptr;
		renderer->commitToScene(item);
	}


	//------------------------- ActorSkinedMeshGPU -------------------------

	ActorSkinedMeshGPU::ActorSkinedMeshGPU(I4Actor* actor, I4ActorElementInfo* info, I4Mesh* _mesh)
	: I4ActorMesh(actor, info, _mesh)
	{
	}

	ActorSkinedMeshGPU::~ActorSkinedMeshGPU()
	{

	}

	bool ActorSkinedMeshGPU::initialize()
	{
		I4ActorMesh::initialize();

		matrixPalette.resize(actor->getBoneCount());

		return true;
	}

	void ActorSkinedMeshGPU::animate(float deltaSec, const I4Matrix4x4& parentTM)
	{
		I4ActorMesh::animate(deltaSec, parentTM);

		assert(actor->getBoneCount() <= matrixPalette.size());
		for (unsigned int i = 0; i < actor->getBoneCount(); ++i)
		{
			matrixPalette[i] = resultTM*actor->getSkinTM(i);
		}
	}

	void ActorSkinedMeshGPU::render(I4DefferedRenderer* renderer, const I4Matrix4x4& parentTM)
	{
		// 스킨드메쉬는 정확한 바운드를 GPU에서 에니메이션 하기전에 알수 없으므로 문제가 생길수 있다.
		// 툴에서 수동으로 또는 미리 계산해서 지정하도록 바꾸자.
		I4MeshInstanceRenderItem item;
		item.mesh = mesh;
		item.shaderMask = I4SHADER_MASK_SKINNING;
		item.boneCount = actor->getBoneCount();
		item.worldAABB = mesh->localAABB.transform(resultTM*parentTM);
		item.worldTM = parentTM;
		item.matrixPalette = &matrixPalette[0];
		renderer->commitToScene(item);
	}

}