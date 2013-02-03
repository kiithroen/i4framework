#include "I4DefferedRenderer.h"
#include "I4VideoDriver.h"
#include "I4ShaderMgr.h"
#include "I4ModelMgr.h"
#include "I4RenderTarget.h"
#include "I4QuadMesh.h"
#include "I4SphereMesh.h"
#include "I4StaticMesh.h"
#include "I4Camera.h"
#include "I4Log.h"
#include "I4Profile.h"

namespace i4graphics
{
	bool I4MeshInstanceRenderItem::operator < (const I4MeshInstanceRenderItem& other) const
	{
		if (meshInstance->diffuseMapID < other.meshInstance->diffuseMapID)
			return true;

		if (meshInstance->specularMapID < other.meshInstance->specularMapID)
			return true;

		if (meshInstance->normalMapID < other.meshInstance->normalMapID)
			return true;

		if (meshInstance->meshID < other.meshInstance->meshID)
			return true;

		return false;
	}

	I4DefferedRenderer::I4DefferedRenderer()
		: shaderMgr(NULL)
		, modelMgr(NULL)
		, rtDiffuse(NULL)
		, rtNormal(NULL)
		, rtDepth(NULL)
		, rtLight(NULL)
		, rtShadow(NULL)
		, quadMesh(NULL)
		, sphereMesh(NULL)
	{
	}


	I4DefferedRenderer::~I4DefferedRenderer(void)
	{
		finalize();
	}

	bool I4DefferedRenderer::initialize(void* _windowID, unsigned int _width, unsigned int _height)
	{
		// video driver
		I4VideoDriver::createVideoDriver(I4VIDEO_DRIVER_MODE_D3D11);
		videoDriver = I4VideoDriver::getVideoDriver();
		if (videoDriver->initialize(_windowID, _width, _height) == false)
		{
			I4LOG_ERROR << L"video driver initalize failed.";
			return false;
		}

		if (videoDriver->setupEnvironment() == false)
		{
			I4LOG_ERROR << L"setup environment failed.";
			return false;
		}
	
		// shader mgr
		if (I4ShaderMgr::addShaderMgr("deffered_g.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_g add failed.";
			return false;
		}
	
		if (I4ShaderMgr::addShaderMgr("deffered_l_directional.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_l_directional add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("deffered_l_point.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_l_point add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("deffered_m.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_m add failed.";
			return false;
		}

		// model mgr
		modelMgr = new I4ModelMgr;

		// render target
		rtDiffuse = videoDriver->createRenderTarget();
		if (rtDiffuse->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target diffuse create failed.";
			return false;
		}

		rtNormal = videoDriver->createRenderTarget();
		if (rtNormal->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target normal create failed.";
			return false;
		}

		rtDepth = videoDriver->createRenderTarget();
		if (rtDepth->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R32_FLOAT) == false)
		{
			I4LOG_ERROR << L"render target depth create failed.";
			return false;
		}

		rtLight = videoDriver->createRenderTarget();
		if (rtLight->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target light create failed.";
			return false;
		}

		rtShadow = videoDriver->createRenderTarget();
		if (rtShadow->createDepthStencil(1024, 1024, I4FORMAT_R32_TYPELESS, I4FORMAT_D32_FLOAT, I4FORMAT_R32_FLOAT) == false)
		{
			I4LOG_ERROR << L"render target shadow create failed.";
			return false;
		}

		// light mesh
		quadMesh = new I4QuadMesh;
		if (quadMesh->create() == false)
		{
			I4LOG_ERROR << L"quad mesh create failed.";
			return false;
		}

		sphereMesh = new I4SphereMesh;
		if (sphereMesh->create() == false)
		{
			I4LOG_ERROR << L"sphere mesh create failed.";
			return false;
		}

		return true;
	}

	void I4DefferedRenderer::finalize()
	{
		delete modelMgr;
		delete sphereMesh;
		delete quadMesh;
		delete rtShadow;
		delete rtLight;
		delete rtDepth;
		delete rtNormal;
		delete rtDiffuse;

		I4ShaderMgr::destroyShaderMgr();
		I4VideoDriver::destroyVideoDriver();
	}

	I4ModelInstance* I4DefferedRenderer::createModelInstance(const char* modelPrototypeName, const char* modelInstanceName)
	{
		return modelMgr->createInstance(modelPrototypeName, modelInstanceName);
	}
	
	void I4DefferedRenderer::destroyModelInstance(I4ModelInstance* modelInstance)
	{
		modelMgr->destroyInstance(modelInstance);
	}

	void I4DefferedRenderer::commitToScene(I4ModelInstance* modelInstance)
	{
		for (unsigned int i = 0; i < modelInstance->getSubCount(); ++i)
		{
			vecSceneMeshInstnaceRenderItem.push_back(I4MeshInstanceRenderItem());
			I4MeshInstanceRenderItem& item = *vecSceneMeshInstnaceRenderItem.rbegin();

			I4MeshInstance& meshInstance = modelInstance->getSubMeshInstance(i);			
			I4Matrix4x4::multiply(item.worldTM, meshInstance.meshLocalTM, modelInstance->getModelTM());
			item.worldAABB = meshInstance.meshLocalAABB.transform(item.worldTM);

			item.meshInstance = &meshInstance;			
		}
	}

	void I4DefferedRenderer::commitToScene(I4DirectionalLight* light)
	{
		vecSceneDirectionalLight.push_back(*light);
	}

	void I4DefferedRenderer::commitToScene(I4PointLight* light)
	{
		vecScenePointLight.push_back(*light);
	}

	void I4DefferedRenderer::preRender(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		videoDriver->beginScene();
	}

	void I4DefferedRenderer::render(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		clearAllRenderTarget();

		renderStageGeometry(camera);
		renderStageLight(camera);
		renderStageMerge(camera);
	}

	void I4DefferedRenderer::postRender(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		videoDriver->endScene();
		vecSceneMeshInstnaceRenderItem.clear();
		vecSceneDirectionalLight.clear();
		vecScenePointLight.clear();
	}

	void I4DefferedRenderer::clearAllRenderTarget()
	{
		PROFILE_THISFUNC;
					
		videoDriver->clearBackBuffer(0, 32, 76);

		videoDriver->clearRenderTarget(rtDiffuse, 0.0f, 0.0f, 0.0f, 0.0f);
		videoDriver->clearRenderTarget(rtNormal, 0.5f, 0.5f, 0.5f, 0.0f);
		videoDriver->clearRenderTarget(rtDepth, 0.0f, 0.0f, 0.0f, 0.0f);
		videoDriver->clearRenderTarget(rtLight, 0.0f, 0.0f, 0.0f, 0.0f);
	}

	void I4DefferedRenderer::renderStageGeometry(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		I4RenderTarget*	renderTargetG[] = { rtDiffuse, rtNormal, rtDepth };
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		cullAndSortMeshInstanceRenderItem(camera);
		renderMeshInstanceRenderItem(camera);		
	}

	void I4DefferedRenderer::cullAndSortMeshInstanceRenderItem(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		vecCulledMeshInstnaceRenderItem.clear();

		I4MeshInstnaceRenderItemVector::iterator itrScene = vecSceneMeshInstnaceRenderItem.begin();
		const I4MeshInstnaceRenderItemVector::iterator itrSceneEnd = vecSceneMeshInstnaceRenderItem.end();

		for (; itrScene != itrSceneEnd; ++itrScene)
		{
			if (camera->isVisibleAABB(itrScene->worldAABB) == true)
			{
				vecCulledMeshInstnaceRenderItem.push_back(*itrScene);
			}
		}

		std::sort(vecCulledMeshInstnaceRenderItem.begin(), vecCulledMeshInstnaceRenderItem.end());
	}

	void I4DefferedRenderer::renderMeshInstanceRenderItem(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("deffered_g.fx");
		//shaderMgr->begin(I4SHADER_MASK_TEX_DIFFUSE|I4SHADER_MASK_TEX_SPECULAR|I4SHADER_MASK_TEX_NORMAL, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));
		/*
		shaderMgr->setMatrix(I4SHADER_MATRIX_PROJECTION, camera->getProjectionMatrix().arr);
		shaderMgr->setFloat(I4SHADER_FLOAT_FAR_DISTANCE, camera->getZFar());

		shaderMgr->setMatrix(I4SHADER_MATRIX_VIEW, camera->getViewMatrix().arr);
		*/
		I4MeshInstance* prevMeshInstance = NULL;
		I4MeshInstance* curMeshInstance = NULL;
		I4StaticMesh* prevMesh = NULL;
		I4StaticMesh* curMesh = NULL;

		I4MeshInstnaceRenderItemVector::iterator itrCulled = vecCulledMeshInstnaceRenderItem.begin();
		const I4MeshInstnaceRenderItemVector::iterator itrCulledEnd = vecCulledMeshInstnaceRenderItem.end();
		for (; itrCulled != itrCulledEnd; ++itrCulled)
		{
			curMeshInstance = itrCulled->meshInstance;					
			bool isChangedDiffuseMap = false;
			bool isChangedSpecularMap = false;
			bool isChangedNormalMap = false;
			bool isChangedMesh = false;

			if (prevMeshInstance == NULL)
			{
				isChangedDiffuseMap = true;
				isChangedSpecularMap = true;
				isChangedNormalMap = true;
			}
			else
			{
				if (prevMeshInstance->diffuseMapID != curMeshInstance->diffuseMapID)
				{
					isChangedDiffuseMap = true;
				}

				if (prevMeshInstance->specularMapID != curMeshInstance->specularMapID)
				{
					isChangedSpecularMap = true;
				}

				if (prevMeshInstance->specularMapID != curMeshInstance->specularMapID)
				{
					isChangedNormalMap = true;								
				}
			}

			if (prevMesh == NULL)
			{
				isChangedMesh = true;
			}
			else
			{
				if (prevMesh->getID() != curMeshInstance->meshID)
				{
					isChangedMesh = true;					
				}
			}
					
			if (isChangedMesh == true)
			{
				if (prevMesh != NULL)
				{
					prevMesh->unbind();
				}

				curMesh = modelMgr->findMesh(curMeshInstance->meshID);

				if (curMesh != NULL)
				{
					curMesh->bind();
				}
			}

			if (curMesh != NULL)
			{
				if (isChangedDiffuseMap == true)
				{
					I4Texture* diffuse = modelMgr->findTexture(curMeshInstance->diffuseMapID);
					//shaderMgr->setTexture(I4SHADER_TEXTURE_DIFFUSE, diffuse);
				}
						
				if (isChangedSpecularMap == true)
				{
					I4Texture* specular = modelMgr->findTexture(curMeshInstance->specularMapID);
					//shaderMgr->setTexture(I4SHADER_TEXTURE_SPECULAR, specular);
				}

				if (isChangedNormalMap == true)
				{
					I4Texture* normal = modelMgr->findTexture(curMeshInstance->normalMapID);
					//shaderMgr->setTexture(I4SHADER_TEXTURE_NORMAL, normal);
				}
				/*
				shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_INTENSITY, curMeshInstance->specularInensity);
				shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_POWER, curMeshInstance->specularPower);		

				shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, itrCulled->worldTM.arr);
				*/

				curMesh->draw();
			}

			prevMeshInstance = curMeshInstance;
			prevMesh = curMesh;
		}
				
		if (curMesh != NULL)
		{
			curMesh->unbind();
		}

		shaderMgr->end();
	}

	void I4DefferedRenderer::renderStageLight(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		videoDriver->setRenderTarget(1, &rtLight, false);
		videoDriver->setBlendMode(I4BLEND_MODE_ADD);

		cullAndSortDirectionalLight(camera);
		renderDirectionalLight(camera);

		cullAndSortPointLight(camera);
		renderPointLight(camera);
	}

	void I4DefferedRenderer::cullAndSortDirectionalLight(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		// 일단 그냥 옮겨 담음. 현재로서는 특별한 정책이 없지만 추후에 너무 많은 라이트가 있으면 잘라낸다던가 병합한다던가...

		vecCulledDirectionalLight.clear();

		I4DirectionalLightVector::iterator itrScene = vecSceneDirectionalLight.begin();
		const I4DirectionalLightVector::iterator itrSceneEnd = vecSceneDirectionalLight.end();

		for (; itrScene != itrSceneEnd; ++itrScene)
		{
			vecCulledDirectionalLight.push_back(*itrScene);
		}
	}

	void I4DefferedRenderer::renderDirectionalLight(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		shaderMgr = I4ShaderMgr::findShaderMgr("deffered_l_directional.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));			
		/*
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, rtNormal);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, rtDepth);
		shaderMgr->setVector(I4SHADER_VECTOR_FAR_TOP_RIGHT, camera->getFarTopRight().xyz);
		*/
		quadMesh->bind();

		I4DirectionalLightVector::iterator itrCulled = vecCulledDirectionalLight.begin();
		const I4DirectionalLightVector::iterator itrCulledEnd = vecCulledDirectionalLight.end();
		for ( ; itrCulled != itrCulledEnd; ++itrCulled)
		{
			const I4DirectionalLight& light = *itrCulled;

			//-----------
			I4Matrix4x4 matLightProj;
			matLightProj.makePerspectiveFovLH(mathutil::PI/4.0f, 1.0f, 1.0f, 1000.0f);
			I4Matrix4x4 matLightView;
			matLightView.makeCameraLookAtLH(-vecSceneDirectionalLight[0].direction*300, -vecSceneDirectionalLight[0].direction*299, I4Vector3(0, 1, 0));
			//matLightView.makeCameraLookAtLH(I4Vector3(8.0f, 2.0f, -20.0f), I4Vector3(-2.0f, 0.0f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));
			I4Matrix4x4 matLightViewProj;
			I4Matrix4x4::multiply(matLightViewProj, matLightView, matLightProj);			
			//shaderMgr->setMatrix(I4SHADER_MATRIX_LIGHT_VIEW_PROJECTION, matLightViewProj.arr);

			I4Matrix4x4 matViewInv;
			camera->getViewMatrix().extractInversePrimitive(matViewInv);
			I4Matrix4x4 matViewInvLightViewProj;
			I4Matrix4x4::multiply(matViewInvLightViewProj, matViewInv, matLightViewProj);			
			//shaderMgr->setMatrix(I4SHADER_MATRIX_VIEWINV_LIGHT_VIEW_PROJECTION, matViewInvLightViewProj.arr);
			//-----------

			const I4Vector3 lightViewDir = camera->getViewMatrix().transformVector(light.direction);
			/*
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_DIRECTION, lightViewDir.xyz);
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_COLOR, light.color.xyz);			
			*/
			quadMesh->draw();
		}
		quadMesh->unbind();
		/*
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_SHADOW, NULL);
		*/

		shaderMgr->end();
	}

	void I4DefferedRenderer::cullAndSortPointLight(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		vecCulledPointLight.clear();

		I4PointLightVector::iterator itrScene = vecScenePointLight.begin();
		const I4PointLightVector::iterator itrSceneEnd = vecScenePointLight.end();

		for (; itrScene != itrSceneEnd; ++itrScene)
		{
			if (camera->isVisibleSphere(itrScene->position, itrScene->radius) == true)
			{
				vecCulledPointLight.push_back(*itrScene);
			}
		}
	}

	void I4DefferedRenderer::renderPointLight(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("deffered_l_point.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS, _countof(I4INPUT_ELEMENTS_POS));
		/*
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, rtNormal);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, rtDepth);
		
		shaderMgr->setMatrix(I4SHADER_MATRIX_PROJECTION, camera->getProjectionMatrix().arr);
		shaderMgr->setMatrix(I4SHADER_MATRIX_VIEW, camera->getViewMatrix().arr);
		shaderMgr->setVector(I4SHADER_VECTOR_FAR_TOP_RIGHT, camera->getFarTopRight().xyz);
		*/
		sphereMesh->bind();

		I4Matrix4x4 matLight;

		I4PointLightVector::iterator itrCulled = vecCulledPointLight.begin();
		const I4PointLightVector::iterator itrCulledEnd = vecCulledPointLight.end();
		for ( ; itrCulled != itrCulledEnd; ++itrCulled)
		{
			const I4PointLight& light = *itrCulled;

			matLight.makeScale(light.radius, light.radius, light.radius);					
			matLight.setTranslation(light.position);

			//shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, matLight.arr);
			
			const I4Vector3 lightViewPos =  camera->getViewMatrix().transformCoord(light.position);

			// 카메라와 라이트의 관계를 찾는데 near plane 을 고려해서 생각해야한다.
			// 안그러면 카메라가 라이트의 외부에 있지만 near plane 안쪽에 있을때 깜박이는 현상이 생긴다.
			float lightRadiusPlusZNear = light.radius + camera->getZNear();
			if (lightViewPos.getLengthSq() - lightRadiusPlusZNear*lightRadiusPlusZNear > 0)
			{
				// 카메라가 라이트의 외부에 있으므로 일반 방식으로 그리면 된다. 
				videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);
			}
			else
			{
				// 카메라가 라이트의 안쪽에 있으므로 뒤집어서 그려야한다.
				videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_BACK);
			}
			/*
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_POSITION, lightViewPos.xyz);
			shaderMgr->setFloat(I4SHADER_FLOAT_LIGHT_RADIUS, light.radius);
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_COLOR, light.color.xyz);			
			*/
			sphereMesh->draw();
		}
		sphereMesh->unbind();
		/*
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, NULL);
		*/

		shaderMgr->end();

		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);
	}

	void I4DefferedRenderer::renderStageMerge(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		videoDriver->resetRenderTarget();
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("deffered_m.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));
		/*
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_LIGHT, rtLight);
		*/
		quadMesh->bind();
		quadMesh->draw();
		quadMesh->unbind();
		/*
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_LIGHT, NULL);
		*/
	}
}