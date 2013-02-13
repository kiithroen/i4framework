#include "I4DefferedRenderer.h"
#include "I4VideoDriver.h"
#include "I4ShaderMgr.h"
#include "I4ActorMgr.h"
#include "I4RenderTarget.h"
#include "I4QuadMesh.h"
#include "I4SphereMesh.h"
#include "I4Mesh.h"
#include "I4Camera.h"
#include "I4Log.h"
#include "I4Profile.h"

namespace i4graphics
{
	
	bool I4MeshInstanceRenderItem::operator < (const I4MeshInstanceRenderItem& other) const
	{
		if (shaderMask < other.shaderMask)
		{
			return true;
		}
		else
		{
			if (shaderMask == other.shaderMask)
			{
				if (mesh < other.mesh)
					return true;
			}
		}

		return false;
	}

	I4DefferedRenderer::I4DefferedRenderer()
		: shaderMgr(nullptr)
		, actorMgr(nullptr)
		, rtDiffuse(nullptr)
		, rtNormal(nullptr)
		, rtDepth(nullptr)
		, rtLight(nullptr)
		, rtShadow(nullptr)
		, quadMesh(nullptr)
		, sphereMesh(nullptr)
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
		if (I4ShaderMgr::addShaderMgr("shader/deffered_g.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_g add failed.";
			return false;
		}
	
		if (I4ShaderMgr::addShaderMgr("shader/deffered_l_directional.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_l_directional add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("shader/deffered_l_point.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_l_point add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("shader/deffered_m.fx") == false)
		{
			I4LOG_ERROR << L"shader deffered_m add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("shader/default.fx") == false)
		{
			I4LOG_ERROR << L"shader default add failed.";
			return false;
		}

		// actor mgr
		actorMgr = new I4ActorMgr;

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
		

		if (cbOnResize_G.create() == false)
			return false;

		if (cbEveryFrame_G.create() == false)
			return false;

		if (cbEachMeshInstance_G_VS.create() == false)
			return false;

		if (cbEachMeshInstance_G_PS.create() == false)
			return false;

		if (cbEachSkinedMesh_G.create() == false)
			return false;

		if (cbOnResize_L_directional.create() == false)
			return false;

		if (cbEachLight_L_directional.create() == false)
			return false;
		
		if (cbOnResize_L_point_VS.create() == false)
			return false;
		if (cbOnResize_L_point_PS.create() == false)
			return false;

		if (cbEveryFrame_L_point.create() == false)
			return false;

		if (cbEachLight_L_point_VS.create() == false)
			return false;

		if (cbEachLight_L_point_PS.create() == false)
			return false;

		return true;
	}

	void I4DefferedRenderer::finalize()
	{
		delete actorMgr;
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

	void I4DefferedRenderer::commitToScene(const I4MeshInstanceRenderItem& item)
	{
			vecSceneMeshInstnaceRenderItem.push_back(item);
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
		I4PROFILE_THISFUNC;

		videoDriver->beginScene();
	}

	void I4DefferedRenderer::render(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;
		clearAllRenderTarget();

		renderStageGeometry(camera);
		renderStageLight(camera);
		renderStageMerge(camera);
	}

	void I4DefferedRenderer::postRender(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		videoDriver->endScene();
		vecSceneMeshInstnaceRenderItem.clear();
		vecSceneDirectionalLight.clear();
		vecScenePointLight.clear();
	}

	void I4DefferedRenderer::clearAllRenderTarget()
	{
		I4PROFILE_THISFUNC;
					
		videoDriver->clearBackBuffer(0, 32, 76);

		videoDriver->clearRenderTarget(rtDiffuse, 0.0f, 0.0f, 0.0f, 0.0f);
		videoDriver->clearRenderTarget(rtNormal, 0.5f, 0.5f, 0.5f, 0.0f);
		videoDriver->clearRenderTarget(rtDepth, 0.0f, 0.0f, 0.0f, 0.0f);
		videoDriver->clearRenderTarget(rtLight, 0.0f, 0.0f, 0.0f, 0.0f);
	}

	void I4DefferedRenderer::renderStageGeometry(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		I4RenderTarget*	renderTargetG[] = { rtDiffuse, rtNormal, rtDepth };
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		cullAndSortMeshInstanceRenderItem(camera);
		renderMeshInstanceRenderItem(camera);		
	}

	void I4DefferedRenderer::cullAndSortMeshInstanceRenderItem(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		vecCulledMeshInstnaceRenderItem.clear();

		for (auto &itr : vecSceneMeshInstnaceRenderItem)
		{
			if (camera->isVisibleAABB(itr.worldAABB) == true)
			{
				vecCulledMeshInstnaceRenderItem.push_back(itr);
			}
		}

		sort(vecCulledMeshInstnaceRenderItem.begin(), vecCulledMeshInstnaceRenderItem.end());
	}

	void I4DefferedRenderer::renderMeshInstanceRenderItem(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		
		I4MeshInstanceRenderItem* prevItem = nullptr;
		I4MeshInstanceRenderItem* curItem = nullptr;
		I4Mesh* prevMesh = nullptr;
		I4Mesh* curMesh = nullptr;
		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deffered_g.fx");

		for (auto &itr : vecCulledMeshInstnaceRenderItem)
		{
			curItem = &itr;			
			
			bool isChangedShader = false;
			bool isChangedDiffuseMap = false;
			bool isChangedSpecularMap = false;
			bool isChangedNormalMap = false;
			bool isChangedMesh = false;

			if (prevItem == nullptr)
			{
				isChangedShader = true;
				isChangedDiffuseMap = true;
				isChangedSpecularMap = true;
				isChangedNormalMap = true;
				isChangedMesh = true;
			}
			else
			{
				if (curItem->shaderMask != prevItem->shaderMask)
				{
					isChangedShader = true;
				}
			}

			if (prevMesh == nullptr)
			{
				isChangedMesh = true;
			}
			else
			{
				if (prevMesh != itr.mesh)
				{
					isChangedMesh = true;					
				}
			}
					
			if (isChangedMesh == true)
			{
				if (prevMesh != nullptr)
				{
					prevMesh->unbind();
				}

				curMesh = itr.mesh;
	
				if (curMesh != nullptr)
				{
					curMesh->bind();
				}

				if (prevMesh != nullptr)
				{
					if (curMesh->diffuseMap != prevMesh->diffuseMap)
					{
						isChangedDiffuseMap = true;
					}

					if (curMesh->specularMap != prevMesh->specularMap)
					{
						isChangedSpecularMap = true;
					}

					if (curMesh->specularMap != prevMesh->specularMap)
					{
						isChangedNormalMap = true;								
					}
				}
			}

			if (curMesh != nullptr)
			{
				if (isChangedShader)
				{
					if (curMesh->skined)
					{
						shaderMgr->begin(curItem->shaderMask, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO));
					}
					else
					{
						shaderMgr->begin(curItem->shaderMask, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));					
					}
					
					shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);

					cbOnResize_G.getData()->projection = camera->getProjectionMatrix(); 
					cbOnResize_G.getData()->farDistance = camera->getZFar();
					shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 0, cbOnResize_G.getBuffer(), cbOnResize_G.getData());

					cbEveryFrame_G.getData()->view = camera->getViewMatrix();
					shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 1, cbEveryFrame_G.getBuffer(), cbEveryFrame_G.getData());
				}

				if (isChangedDiffuseMap == true)
				{
					shaderMgr->setTexture(0, curMesh->diffuseMap);
				}
						
				if (isChangedSpecularMap == true)
				{
					shaderMgr->setTexture(1, curMesh->specularMap);
				}

				if (isChangedNormalMap == true)
				{
					shaderMgr->setTexture(2, curMesh->normalMap);
				}

				cbEachMeshInstance_G_VS.getData()->world = itr.worldTM;
				shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 2, cbEachMeshInstance_G_VS.getBuffer(), cbEachMeshInstance_G_VS.getData());

				cbEachMeshInstance_G_PS.getData()->specularIntensity = 0.7f;
				cbEachMeshInstance_G_PS.getData()->specularPower = 16.0f;
				shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 3, cbEachMeshInstance_G_PS.getBuffer(), cbEachMeshInstance_G_PS.getData());				

				if (curItem->boneCount != 0)
				{
					for (unsigned int i = 0; i < itr.boneCount; ++i)
					{
						cbEachSkinedMesh_G.getData()->matrixPalette[i] = itr.matrixPalette[i];
					}
					shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 4, cbEachSkinedMesh_G.getBuffer(), cbEachSkinedMesh_G.getData());
				}

				curMesh->draw();
			}

			prevItem = curItem;
			prevMesh = curMesh;
		}
				
		if (curMesh != nullptr)
		{
			curMesh->unbind();
		}

		shaderMgr->end();
	}

	void I4DefferedRenderer::renderStageLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		videoDriver->setRenderTarget(1, &rtLight, nullptr);
		videoDriver->setBlendMode(I4BLEND_MODE_ADD);

		cullAndSortDirectionalLight(camera);
		renderDirectionalLight(camera);

		cullAndSortPointLight(camera);
		renderPointLight(camera);
	}

	void I4DefferedRenderer::cullAndSortDirectionalLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		// 일단 그냥 옮겨 담음. 현재로서는 특별한 정책이 없지만 추후에 너무 많은 라이트가 있으면 잘라낸다던가 병합한다던가...

		vecCulledDirectionalLight.clear();
		for (auto &itr : vecSceneDirectionalLight)
		{
			vecCulledDirectionalLight.push_back(itr);
		}
	}

	void I4DefferedRenderer::renderDirectionalLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		shaderMgr = I4ShaderMgr::findShaderMgr("shader/deffered_l_directional.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));	
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);
		shaderMgr->setSamplerState(1, I4SAMPLER_STATE_POINT);

		shaderMgr->setRenderTarget(0, rtDiffuse);
		shaderMgr->setRenderTarget(1, rtNormal);
		shaderMgr->setRenderTarget(2, rtDepth);

		cbOnResize_L_directional.getData()->farTopRight = camera->getFarTopRight();
		shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 0, cbOnResize_L_directional.getBuffer(), cbOnResize_L_directional.getData());

		quadMesh->bind();

		for (auto &itr : vecCulledDirectionalLight)
		{
			const I4DirectionalLight& light = itr;

			const I4Vector3 lightViewDir = camera->getViewMatrix().transformVector(light.direction);

			cbEachLight_L_directional.getData()->lightViewDirection = lightViewDir;
			cbEachLight_L_directional.getData()->lightColor = light.color;
			shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 1, cbEachLight_L_directional.getBuffer(), cbEachLight_L_directional.getData());

			quadMesh->draw();
		}
		quadMesh->unbind();

		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);
		shaderMgr->setRenderTarget(2, nullptr);

		shaderMgr->end();
	}

	void I4DefferedRenderer::cullAndSortPointLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		vecCulledPointLight.clear();
		
		for (auto &itr : vecScenePointLight)
		{
			if (camera->isVisibleSphere(itr.position, itr.radius) == true)
			{
				vecCulledPointLight.push_back(itr);
			}
		}
	}

	void I4DefferedRenderer::renderPointLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deffered_l_point.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS, _countof(I4INPUT_ELEMENTS_POS));
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);
		shaderMgr->setSamplerState(1, I4SAMPLER_STATE_POINT);

		shaderMgr->setRenderTarget(0, rtDiffuse);
		shaderMgr->setRenderTarget(1, rtNormal);
		shaderMgr->setRenderTarget(2, rtDepth);
		
		cbOnResize_L_point_VS.getData()->projection = camera->getProjectionMatrix();
		shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 0, cbOnResize_L_point_VS.getBuffer(), cbOnResize_L_point_VS.getData());

		cbOnResize_L_point_PS.getData()->farTopRight = camera->getFarTopRight();
		shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 1, cbOnResize_L_point_PS.getBuffer(), cbOnResize_L_point_PS.getData());

		cbEveryFrame_L_point.getData()->view = camera->getViewMatrix();
		shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 2, cbEveryFrame_L_point.getBuffer(), cbEveryFrame_L_point.getData());

		sphereMesh->bind();

		I4Matrix4x4 matLight;

		for (auto &itr : vecCulledPointLight)
		{
			const I4PointLight& light = itr;

			matLight.makeScale(light.radius, light.radius, light.radius);
			matLight.setTranslation(light.position);

			
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

			cbEachLight_L_point_VS.getData()->world = matLight;
			shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 3, cbEachLight_L_point_VS.getBuffer(), cbEachLight_L_point_VS.getData());


			cbEachLight_L_point_PS.getData()->lightPosition = lightViewPos;
			cbEachLight_L_point_PS.getData()->lightRadius = light.radius;
			cbEachLight_L_point_PS.getData()->lightColor = light.color;
			shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 4, cbEachLight_L_point_PS.getBuffer(), cbEachLight_L_point_PS.getData());

			sphereMesh->draw();
		}
		sphereMesh->unbind();
		
		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);
		shaderMgr->setRenderTarget(2, nullptr);
		

		shaderMgr->end();

		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);
	}

	void I4DefferedRenderer::renderStageMerge(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		videoDriver->resetRenderTarget();
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deffered_m.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);

		shaderMgr->setRenderTarget(0, rtDiffuse);
		shaderMgr->setRenderTarget(1, rtLight);
		
		quadMesh->bind();
		quadMesh->draw();
		quadMesh->unbind();
		
		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);		
	}
}