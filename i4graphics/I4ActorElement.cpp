#include "I4ActorElement.h"
#include "I4StaticMesh.h"
#include "I4AnimationController.h"
#include "I4Actor.h"
#include "I4ShaderMgr.h"
#include "I4Material.h"
#include "I4VideoDriver.h"
#include "I4Log.h"
#include "I4Camera.h"
#include "I4DefferedRenderer.h"

namespace i4graphics
{
	//------------------------- I4ActorElement ---------------------

	I4ActorElement::I4ActorElement(I4Actor* _actor, I4ActorElementInfo* info)
		: actor(_actor)
		, elementInfo(info)
		, parentElement(NULL)
		, aniController(NULL)
	{
	}

	I4ActorElement::~I4ActorElement()
	{
		delete aniController;
	}

	void I4ActorElement::registerAni(const char* name, KeyFrameSet* keyFrameSet)
	{
		if (aniController == NULL)
		{
			aniController = new AnimationController;
		}
		
		aniController->addTrack(name, keyFrameSet);
	}

	void I4ActorElement::playAni(const char* name)
	{
		if (aniController == NULL)
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
		if (aniController == NULL)
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

		elementInfo->worldTM.extractInverse(worldInverseTM);;

		return true;
	}

	void I4ActorBone::render(I4DefferedRenderer* renderer, const I4Matrix4x4& parentTM)
	{
	}

	//------------------------- I4ActorMesh -------------------------

	I4ActorMesh::I4ActorMesh(I4Actor* actor, I4ActorElementInfo* info, I4StaticMesh* _mesh)
	: I4ActorElement(actor, info)
	, mesh(_mesh)
	{
	}

	I4ActorMesh::~I4ActorMesh()
	{
	}

	//------------------------- ActorRigidMesh -------------------------

	ActorRigidMesh::ActorRigidMesh(I4Actor* actor, I4ActorElementInfo* info, I4StaticMesh* _mesh)
	: I4ActorMesh(actor, info, _mesh)
	{
	}

	ActorRigidMesh::~ActorRigidMesh()
	{
		
	}

	void ActorRigidMesh::render(I4DefferedRenderer* renderer, const I4Matrix4x4& parentTM)
	{
		/*
		I4ShaderMgr* shaderMgr = I4ShaderMgr::getShaderMgr();
		unsigned int mask = SHADER_PER_PIXEL_LIGHTING_MASK;
		if (mesh->getDiffuseMap() != NULL)
		{
			mask |= SHADER_DIFFUSE_MAP_MASK;
		}
		if (mesh->getSpecularMap() != NULL)
		{
			mask |= SHADER_SPECULAR_MAP_MASK;
		}
		if (mesh->getNormalMap() != NULL)
		{
			mask |= SHADER_NORMAL_MAP_MASK;
		}

		if (shaderMgr->begin(mask) == true)
		{
			const I4Matrix4x4 worldTM = resultTM*parentTM;
			shaderMgr->setMatrix4x4("u_worldMatrix", &worldTM);
			const I4Matrix4x4 worldViewProjectionMatrix = worldTM*VideoDriver::getVideoDriver()->getCamera().getViewProjectionMatrix();
			shaderMgr->setMatrix4x4("u_worldViewProjectionMatrix", &worldViewProjectionMatrix);

			// 월드에서의 광원
			I4Vector3 lightDir = I4Vector3(1.0f, -1.0f, 1.0f);
			lightDir.normalize();
			shaderMgr->setVector3("u_worldLightDir", &lightDir);

			// 월드에서의 시점
			I4Vector3 eyePos = VideoDriver::getVideoDriver()->getCamera().getPosition();
			shaderMgr->setVector3("u_worldEyePos", &eyePos);

			I4Vector3 lightColor = I4Vector3(1.0f, 1.0f, 1.0f);
			shaderMgr->setVector3("u_lightColor", &lightColor);

			I4Vector3 ambientColor = I4Vector3(0.45f, 0.45f, 0.45f);
			shaderMgr->setVector3("u_ambientColor", &ambientColor);

			shaderMgr->setTexture(0, "u_diffuseMap", mesh->getDiffuseMap());
			shaderMgr->setTexture(1, "u_specularMap", mesh->getSpecularMap());
			shaderMgr->setTexture(2, "u_normalMap", mesh->getNormalMap());
			
			shaderMgr->commitChanges();

			mesh->draw();
			shaderMgr->end();
		}
		*/
	}


	//------------------------- ActorSkinedMeshGPU -------------------------

	ActorSkinedMeshGPU::ActorSkinedMeshGPU(I4Actor* actor, I4ActorElementInfo* info, I4StaticMesh* _mesh)
	: I4ActorMesh(actor, info, _mesh)
	{
	}

	ActorSkinedMeshGPU::~ActorSkinedMeshGPU()
	{

	}

	bool ActorSkinedMeshGPU::initialize()
	{
		I4ActorMesh::initialize();

		matrixPalette.resize(80);

		return true;
	}

	void ActorSkinedMeshGPU::animate(float deltaSec, const I4Matrix4x4& parentTM)
	{
		I4ActorMesh::animate(deltaSec, parentTM);

		assert(actor->getBoneCount() <= matrixPalette.size());

		// 이 부분 저장 안하고 셰이더에 직접 루프돌면서 하나하나 넣어주는게 더 나을까?
		for (unsigned int i = 0; i < actor->getBoneCount(); ++i)
		{
			matrixPalette[i] = resultTM * actor->getSkinTM(i);
		}
	}

	void ActorSkinedMeshGPU::render(I4DefferedRenderer* renderer, const I4Matrix4x4& parentTM)
	{
		I4MeshInstanceRenderItem item;
		item.mesh = mesh;
		item.worldAABB = mesh->localAABB.transform(parentTM);
		item.worldTM = parentTM;
		renderer->commitToScene(item);
		/*
		I4ShaderMgr* shaderMgr = I4ShaderMgr::getShaderMgr();
		unsigned int mask = SHADER_PER_PIXEL_LIGHTING_MASK|SHADER_GPU_SKINNING;
		if (mesh->getDiffuseMap() != NULL)
		{
			mask |= SHADER_DIFFUSE_MAP_MASK;
		}
		if (mesh->getSpecularMap() != NULL)
		{
			mask |= SHADER_SPECULAR_MAP_MASK;
		}
		if (mesh->getNormalMap() != NULL)
		{
			mask |= SHADER_NORMAL_MAP_MASK;
		}

		if (shaderMgr->begin(mask) == true)
		{
			const I4Matrix4x4 worldTM = parentTM;
			shaderMgr->setMatrix4x4("u_worldMatrix", &worldTM);
			const I4Matrix4x4 worldViewProjectionMatrix = worldTM*VideoDriver::getVideoDriver()->getCamera().getViewProjectionMatrix();
			shaderMgr->setMatrix4x4("u_worldViewProjectionMatrix", &worldViewProjectionMatrix);

			// 월드에서의 광원
			I4Vector3 worldLightDir = I4Vector3(1.0f, 0.0f, 0.0f);//I4Vector3(1.0f, -1.0f, 1.0f);
			worldLightDir.normalize();
			shaderMgr->setVector3("u_worldLightDir", &worldLightDir);

			// 월드에서의 시점
			I4Vector3 worldEyePos = VideoDriver::getVideoDriver()->getCamera().getPosition();
			shaderMgr->setVector3("u_worldEyePos", &worldEyePos);

			I4Vector3 lightColor = I4Vector3(1.0f, 1.0f, 1.0f);
			shaderMgr->setVector3("u_lightColor", &lightColor);

			I4Vector3 ambientColor = I4Vector3(0.0f, 0.0f, 0.0f);
			shaderMgr->setVector3("u_ambientColor", &ambientColor);

			shaderMgr->setTexture(0, "u_diffuseMap", mesh->getDiffuseMap());
			shaderMgr->setTexture(1, "u_specularMap", mesh->getSpecularMap());
			shaderMgr->setTexture(2, "u_normalMap", mesh->getNormalMap());

			shaderMgr->setMatrix4x4Array("u_matrices", &matrixPalette[0], 80);	

			shaderMgr->commitChanges();

			mesh->draw();
			shaderMgr->end();
		}
		*/
	}

}