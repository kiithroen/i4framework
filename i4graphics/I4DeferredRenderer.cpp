#include "stdafx.h"
#include "I4DeferredRenderer.h"
#include "I4VideoDriver.h"
#include "I4ShaderMgr.h"
#include "I4ActorMgr.h"
#include "I4RenderTarget.h"
#include "I4QuadMesh.h"
#include "I4SphereMesh.h"
#include "I4Mesh.h"
#include "I4Material.h"
#include "I4Camera.h"
#include "I4Log.h"
#include "I4Profile.h"
#include "I4TextureMgr.h"

namespace i4graphics
{
	
	bool I4MeshRenderItem::operator < (const I4MeshRenderItem& other) const
	{
		if (shaderMask < other.shaderMask)									// 셰이더 우선으로 정렬하고
		{
			return true;
		}
		else
		{
			if (shaderMask == other.shaderMask)								// 셰이더가 같으면
			{
				if (material->diffuseMap < other.material->diffuseMap)		// 텍스쳐 우선으로 정렬한다. 다만 디퓨즈맵이 다르면 다른것도 다를 가능성이 높기에 디퓨즈맵만 비교한다
				{
					return true;
				}
				else
				{
					if (material->diffuseMap == other.material->diffuseMap)	// 디퓨즈맵이 같으면 
					{
						if (mesh < other.mesh)								// 메시 우선으로 정렬한다.
							return true;
					}
				}
			}
		}

		return false;
	}

	I4DeferredRenderer::I4DeferredRenderer()
		: shaderMgr(nullptr)
		, rtDiffuse(nullptr)
		, rtSpecular(nullptr)
		, rtNormal(nullptr)
		, rtDepth(nullptr)
		, rtLight(nullptr)
		, rtShadow(nullptr)
		, quadMesh(nullptr)
		, sphereMesh(nullptr)
		, wireMode(false)
		, cascadeSize(1024)
		, cascadeLevel(4)
	{
	}


	I4DeferredRenderer::~I4DeferredRenderer(void)
	{
		finalize();
	}

	bool I4DeferredRenderer::initialize(void* _windowID, unsigned int _width, unsigned int _height)
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
		if (I4ShaderMgr::addShaderMgr("shader/deferred_g.fx") == false)
		{
			I4LOG_ERROR << L"shader deferred_g add failed.";
			return false;
		}
	
		if (I4ShaderMgr::addShaderMgr("shader/deferred_l_directional.fx") == false)
		{
			I4LOG_ERROR << L"shader deferred_l_directional add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("shader/deferred_l_point.fx") == false)
		{
			I4LOG_ERROR << L"shader deferred_l_point add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("shader/deferred_m.fx") == false)
		{
			I4LOG_ERROR << L"shader deferred_m add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("shader/shadowmap.fx") == false)
		{
			I4LOG_ERROR << L"shader shadowmap add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("shader/default.fx") == false)
		{
			I4LOG_ERROR << L"shader default add failed.";
			return false;
		}
		
		// render target
		rtDiffuse = videoDriver->createRenderTarget();
		if (rtDiffuse->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target diffuse create failed.";
			return false;
		}

		rtSpecular = videoDriver->createRenderTarget();
		if (rtSpecular->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target specular create failed.";
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
		if (rtShadow->createDepthStencil(cascadeSize*cascadeLevel, cascadeSize, I4FORMAT_R32_TYPELESS, I4FORMAT_D32_FLOAT, I4FORMAT_R32_FLOAT) == false)
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
		if (sphereMesh->create(1.0f, 16, 16) == false)
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

		if (cbEachAllMesh_S_VS.create() == false)
			return false;

		if (cbEachSkinedMesh_S_VS.create() == false)
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

	void I4DeferredRenderer::finalize()
	{
		delete sphereMesh;
		delete quadMesh;
		delete rtShadow;
		delete rtLight;
		delete rtDepth;
		delete rtNormal;
		delete rtSpecular;
		delete rtDiffuse;

		I4ShaderMgr::destroyShaderMgr();
		I4VideoDriver::destroyVideoDriver();
	}

	void I4DeferredRenderer::commitToScene(const I4MeshRenderItem& item)
	{
		vecSceneMeshRenderItem.push_back(item);
	}

	void I4DeferredRenderer::commitToScene(I4DirectionalLight* light)
	{
		vecSceneDirectionalLight.push_back(*light);
	}

	void I4DeferredRenderer::commitToScene(I4PointLight* light)
	{
		vecScenePointLight.push_back(*light);
	}

	void I4DeferredRenderer::preRender(I4Camera* camera)
	{
		videoDriver->beginScene();
	}

	void I4DeferredRenderer::render(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;
		clearAllRenderTarget();

		I4RenderTarget*	renderTargetG[] = { rtShadow };
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG, rtShadow);
		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_NONE);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		renderStageGeometry(camera);
		renderStageShadow(camera);
		renderStageLight(camera);
		renderStageMerge(camera);
	}

	void I4DeferredRenderer::postRender(I4Camera* camera)
	{
		videoDriver->endScene();
		vecSceneMeshRenderItem.clear();
		vecSceneDirectionalLight.clear();
		vecScenePointLight.clear();
	}

	void I4DeferredRenderer::clearAllRenderTarget()
	{
		I4PROFILE_THISFUNC;
					
		videoDriver->clearBackBuffer(0, 32, 76);

		videoDriver->clearRenderTarget(rtDiffuse, 0.0f, 32.0f/255.0f, 76.0f/255.0f, 0.0f);
		videoDriver->clearRenderTarget(rtSpecular, 0.0f, 0.0f, 0.0f, 0.0f);
		videoDriver->clearRenderTarget(rtNormal, 0.5f, 0.5f, 0.5f, 0.0f);
		videoDriver->clearRenderTarget(rtDepth, 0.0f, 0.0f, 0.0f, 0.0f);
		videoDriver->clearRenderTarget(rtLight, 0.0f, 0.0f, 0.0f, 0.0f);

		videoDriver->clearDepthStencil(rtShadow, 1.0f, 0);
	}

	void I4DeferredRenderer::renderStageGeometry(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		I4RenderTarget*	renderTargetG[] = { rtDiffuse, rtSpecular, rtNormal, rtDepth, rtLight };
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		cullAndSortMeshGeometryRenderItem(camera);
		renderMeshGeometryRenderItem(camera);		
	}

	void I4DeferredRenderer::cullAndSortMeshGeometryRenderItem(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		vecCulledMeshRenderItem.clear();

		for (auto& itr : vecSceneMeshRenderItem)
		{
			if (camera->isVisibleAABB(itr.worldAABB))
			{
				vecCulledMeshRenderItem.push_back(itr);
			}
		}

		sort(vecCulledMeshRenderItem.begin(), vecCulledMeshRenderItem.end());
	}

	
	void I4DeferredRenderer::renderMeshGeometryRenderItem(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		I4MeshRenderItem* prevItem = nullptr;
		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deferred_g.fx");

		for (auto& itr : vecCulledMeshRenderItem)
		{
			bool isChangedShader = false;
			bool isChangedDiffuseMap = false;
			bool isChangedSpecularMap = false;
			bool isChangedNormalMap = false;
			bool isChangedMesh = false;
			bool isChangedTwoSide = false;

			if (prevItem == nullptr)
			{
				isChangedShader = true;
				isChangedDiffuseMap = true;
				isChangedSpecularMap = true;
				isChangedNormalMap = true;
				isChangedMesh = true;
				isChangedTwoSide = true;
			}
			else
			{
				if (itr.shaderMask != prevItem->shaderMask)
				{
					isChangedShader = true;
				}
				
				if (itr.material->diffuseMap != prevItem->material->diffuseMap)
				{
					isChangedDiffuseMap = true;
				}

				if (itr.material->specularMap != prevItem->material->specularMap)
				{
					isChangedSpecularMap = true;
				}

				if (itr.material->specularMap != prevItem->material->specularMap)
				{
					isChangedNormalMap = true;
				}

				if (itr.material->twoSide != prevItem->material->twoSide)
				{
					isChangedTwoSide = true;
				}

				if (prevItem->mesh != itr.mesh)
				{
					isChangedMesh = true;					
				}
			}

			if (isChangedShader)
			{
				if (itr.mesh->skined)
				{
					shaderMgr->begin(itr.shaderMask, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO));
				}
				else
				{
					shaderMgr->begin(itr.shaderMask, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));					
				}
					
				shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);

				cbOnResize_G.getData()->projection = camera->getProjectionMatrix(); 
				cbOnResize_G.getData()->farDistance = camera->getZFar();
				shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 0, cbOnResize_G.getBuffer(), cbOnResize_G.getData());

				cbEveryFrame_G.getData()->view = camera->getViewMatrix();
				shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 1, cbEveryFrame_G.getBuffer(), cbEveryFrame_G.getData());
			}
		
			if (isChangedMesh)
			{
				if (prevItem != nullptr)
				{
					prevItem->mesh->unbind();
				}
					
				itr.mesh->bind();
			}

			if (isChangedTwoSide)
			{
				int mode = I4RASTERIZER_MODE_SOLID_FRONT;
				if (itr.material->twoSide)
				{
					mode = I4RASTERIZER_MODE_SOLID_NONE;
				}
				
				if (wireMode)
				{
					mode += I4RASTERIZER_MODE_WIRE_NONE;
				}

				videoDriver->setRasterizerMode((I4RasterizerMode)mode);
			}

			if (isChangedDiffuseMap)
			{
				I4Texture* texture = I4VideoDriver::getVideoDriver()->getTextureMgr()->find(itr.material->diffuseMap);
				shaderMgr->setTexture(0, texture);
			}
						
			if (isChangedSpecularMap)
			{
				I4Texture* texture = I4VideoDriver::getVideoDriver()->getTextureMgr()->find(itr.material->specularMap);
				shaderMgr->setTexture(1, texture);
			}

			if (isChangedNormalMap)
			{
				I4Texture* texture = I4VideoDriver::getVideoDriver()->getTextureMgr()->find(itr.material->normalMap);
				shaderMgr->setTexture(2, texture);
			}				

			cbEachMeshInstance_G_VS.getData()->world = itr.worldTM;
			shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 2, cbEachMeshInstance_G_VS.getBuffer(), cbEachMeshInstance_G_VS.getData());

			cbEachMeshInstance_G_PS.getData()->ambient = itr.material->ambient;
			cbEachMeshInstance_G_PS.getData()->specularGlossiness = itr.material->specularGlossiness;
			cbEachMeshInstance_G_PS.getData()->specularPower = itr.material->specularPower;
			shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 3, cbEachMeshInstance_G_PS.getBuffer(), cbEachMeshInstance_G_PS.getData());				

			if (itr.boneCount != 0)
			{
				for (unsigned int i = 0; i < itr.boneCount; ++i)
				{
					cbEachSkinedMesh_G.getData()->matrixPalette[i] = itr.matrixPalette[i];
				}
				shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 4, cbEachSkinedMesh_G.getBuffer(), cbEachSkinedMesh_G.getData());
			}

			itr.mesh->draw();

			prevItem = &itr;
		}
				
		if (prevItem != nullptr)
		{
			prevItem->mesh->unbind();
		}

		shaderMgr->end();
	}

	void I4DeferredRenderer::renderStageShadow(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		I4RenderTarget*	renderTargetG[] = { rtShadow };
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG, rtShadow);
		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_NONE);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);
		
		lightPerspectiveCamera.setLookAt(vecSceneDirectionalLight[0].direction*-0.2f, vecSceneDirectionalLight[0].direction, I4VECTOR3_AXISY);
		lightPerspectiveCamera.setPerspectiveFov(I4PI/4, 1.0f, 0.1f, 1000.0f);

		I4Camera tempSplitCamera;
		tempSplitCamera.setViewMatrix(camera->getViewMatrix());

		float partition[] = { 0.1f, 3.6f, 7.1f, 20, 50 };	// 0, 1 레벨은 거의 같은 크기로 나눠줘야 경계 현상이 안보인다.
		for (int i = 0; i < cascadeLevel; ++i)
		{
			splitLightOrthoCamera[i].setViewMatrix(lightPerspectiveCamera.getViewMatrix());

			videoDriver->setViewport(i*cascadeSize, 0, cascadeSize, cascadeSize);
			
			tempSplitCamera.setPerspectiveFov(camera->getFovY(), camera->getAspect(), partition[i], partition[i+1]);
																											
			I4Vector3 corners[8];
			tempSplitCamera.extractCorners(corners);
			
			I4Matrix4x4 matInvView;
			tempSplitCamera.getViewMatrix().extractInverse(matInvView);

			I4Matrix4x4 matToLightView = matInvView*splitLightOrthoCamera[i].getViewMatrix();

			for (int j = 0; j < 8; ++j)
			{
				corners[j] = matToLightView.transformCoord(corners[j]);				
			}

			I4AABB aabbInLightSpace;
			for (int j = 0; j < 8; ++j)
			{
				aabbInLightSpace.merge(corners[j]);
			}

			I4Sphere spereInLightSpace;
			spereInLightSpace.fromAABB(aabbInLightSpace);

			// 정확하게는 라이트의 위치에서 뷰프러스텀사이에 끼어드는 메시를 찾아서 zFar, zNear를 구해야하지만 임의로 구를 만들어서 구한다.
			I4Vector3 vMin = spereInLightSpace.center - I4VECTOR3_ONE*spereInLightSpace.radius;
			I4Vector3 vMax = spereInLightSpace.center + I4VECTOR3_ONE*spereInLightSpace.radius;

			splitLightOrthoCamera[i].setOrthoOffCenter(aabbInLightSpace.minEdge.x, aabbInLightSpace.maxEdge.x, aabbInLightSpace.minEdge.y, aabbInLightSpace.maxEdge.y, vMin.z, vMax.z);

			cullAndSortMeshShadowRenderItem(&splitLightOrthoCamera[i]);
			renderMeshShadowRenderItem(&splitLightOrthoCamera[i]);		
		}

		videoDriver->resetViewport();
	}

	void I4DeferredRenderer::cullAndSortMeshShadowRenderItem(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		vecCulledMeshRenderItem.clear();

		for (auto& itr : vecSceneMeshRenderItem)
		{
			if (itr.shadowCaster == true && camera->isVisibleAABB(itr.worldAABB) == true)
			{
				vecCulledMeshRenderItem.push_back(itr);
			}
		}

		sort(vecCulledMeshRenderItem.begin(), vecCulledMeshRenderItem.end());
	}

	void I4DeferredRenderer::renderMeshShadowRenderItem(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		I4MeshRenderItem* prevItem = nullptr;
		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/shadowmap.fx");

		for (auto& itr : vecCulledMeshRenderItem)
		{
			bool isChangedShader = false;
			bool isChangedMesh = false;

			if (prevItem == nullptr)
			{
				isChangedShader = true;
				isChangedMesh = true;
			}
			else
			{
				if (itr.shaderMask != prevItem->shaderMask)
				{
					isChangedShader = true;
				}
				
				if (prevItem->mesh != itr.mesh)
				{
					isChangedMesh = true;					
				}
			}

			if (isChangedShader)
			{
				if (itr.mesh->skined)
				{
					shaderMgr->begin(I4SHADER_MASK_SKINNING, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN_SKININFO));
				}
				else
				{
					shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));					
				}
			}
		
			if (isChangedMesh)
			{
				if (prevItem != nullptr)
				{
					prevItem->mesh->unbind();
				}
					
				itr.mesh->bind();
			}
	
			cbEachAllMesh_S_VS.getData()->worldViewProj = itr.worldTM*camera->getViewProjectionMatrix(); 
			shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 0, cbEachAllMesh_S_VS.getBuffer(), cbEachAllMesh_S_VS.getData());			
			if (itr.boneCount != 0)
			{
				for (unsigned int i = 0; i < itr.boneCount; ++i)
				{
					cbEachSkinedMesh_S_VS.getData()->matrixPalette[i] = itr.matrixPalette[i];
				}
				shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_VS, 1, cbEachSkinedMesh_S_VS.getBuffer(), cbEachSkinedMesh_S_VS.getData());
			}
			itr.mesh->draw();

			prevItem = &itr;
		}
				
		if (prevItem != nullptr)
		{
			prevItem->mesh->unbind();
		}

		shaderMgr->end();
	}

	void I4DeferredRenderer::renderStageLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		videoDriver->setRenderTarget(1, &rtLight, nullptr);
		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);
		videoDriver->setBlendMode(I4BLEND_MODE_ADD);

		cullAndSortDirectionalLight(camera);
		renderDirectionalLight(camera);

		cullAndSortPointLight(camera);
		renderPointLight(camera);
	}

	void I4DeferredRenderer::cullAndSortDirectionalLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		// 일단 그냥 옮겨 담음. 현재로서는 특별한 정책이 없지만 추후에 너무 많은 라이트가 있으면 잘라낸다던가 병합한다던가...

		vecCulledDirectionalLight.clear();
		for (auto& itr : vecSceneDirectionalLight)
		{
			vecCulledDirectionalLight.push_back(itr);
		}
	}

	void I4DeferredRenderer::renderDirectionalLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		shaderMgr = I4ShaderMgr::findShaderMgr("shader/deferred_l_directional.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));	
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);
		shaderMgr->setSamplerState(1, I4SAMPLER_STATE_POINT);
		shaderMgr->setSamplerState(2, I4SAMPLER_STATE_SHADOW);

		shaderMgr->setRenderTarget(0, rtDiffuse);
		shaderMgr->setRenderTarget(1, rtNormal);
		shaderMgr->setRenderTarget(2, rtDepth);
		shaderMgr->setRenderTarget(3, rtShadow);

		cbOnResize_L_directional.getData()->farTopRight = camera->getFarTopRight();
		shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 0, cbOnResize_L_directional.getBuffer(), cbOnResize_L_directional.getData());

		quadMesh->bind();

		for (auto& itr : vecCulledDirectionalLight)
		{
			const I4DirectionalLight& light = itr;

			const I4Vector3 lightViewDir = camera->getViewMatrix().transformVector(light.direction);

			I4Matrix4x4 matViewInv;
			camera->getViewMatrix().extractInverse(matViewInv);

			for (int i = 0; i < cascadeLevel; ++i)
			{
				cbEachLight_L_directional.getData()->viewInvLightViewProjection[i] = matViewInv*splitLightOrthoCamera[i].getViewProjectionMatrix();
			}
			cbEachLight_L_directional.getData()->lightViewDirection = lightViewDir;
			cbEachLight_L_directional.getData()->lightColor = light.color;
			shaderMgr->setConstantBuffer(I4SHADER_PROGRAM_TYPE_PS, 1, cbEachLight_L_directional.getBuffer(), cbEachLight_L_directional.getData());

			quadMesh->draw();
		}
		quadMesh->unbind();

		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);
		shaderMgr->setRenderTarget(2, nullptr);
		shaderMgr->setRenderTarget(3, nullptr);

		shaderMgr->end();
	}

	void I4DeferredRenderer::cullAndSortPointLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		vecCulledPointLight.clear();
		
		for (auto& itr : vecScenePointLight)
		{
			if (camera->isVisibleSphere(itr.position, itr.radius))
			{
				vecCulledPointLight.push_back(itr);
			}
		}
	}

	void I4DeferredRenderer::renderPointLight(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deferred_l_point.fx");
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

		for (auto& itr : vecCulledPointLight)
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
	}

	void I4DeferredRenderer::renderStageMerge(I4Camera* camera)
	{
		I4PROFILE_THISFUNC;

		videoDriver->resetRenderTarget();
		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deferred_m.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);

		shaderMgr->setRenderTarget(0, rtDiffuse);
		shaderMgr->setRenderTarget(1, rtSpecular);
		shaderMgr->setRenderTarget(2, rtLight);
		
		quadMesh->bind();
		quadMesh->draw();
		quadMesh->unbind();
		
		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);
		shaderMgr->setRenderTarget(2, nullptr);

		shaderMgr->end();
	}
}