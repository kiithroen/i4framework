#include "stdafx.h"
#include "I4DeferredRenderer.h"
#include "I4VideoDriver.h"
#include "I4ShaderMgr.h"
#include "I4ModelMgr.h"
#include "I4RenderTarget.h"
#include "I4ScreenQuadMesh.h"
#include "I4PointLightMesh.h"
#include "I4TriangleMesh.h"
#include "I4Material.h"
#include "I4Camera.h"
#include "I4Log.h"
#include "I4Profile.h"
#include "I4TextureMgr.h"
#include "I4LineBatch.h"
#include "I4FrameTimer.h"

namespace i4graphics
{
	// TODO : 그림자 그릴때와 일반일때 정렬알고리즘을 달리하자.
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
		, screenQuadMesh(nullptr)
		, pointLightMesh(nullptr)
		, shadowSplitSize(1024)
	{
		static const float defaultShadowSplitZ[] = { 3.5f, 7.0f, 20.0f, 50.0f };	// 0, 1 레벨은 거의 같은 크기로 나눠줘야 경계 현상이 안보인다
		static const float defaultShadowBias[] = { 0.001f, 0.001f, 0.002f, 0.003f };
		for (int  i = 0; i < 4; ++i)
		{
			shadowSplitZ[i] = defaultShadowSplitZ[i];
			shadowBias[i] = defaultShadowBias[i];
		}


		// 디폴트 썬라이트 세팅.
		/*
		sunset=182,126,91
		noon=192,191,173
		clouds, haze=189,190,192
		overcast=174,183,190
		*/
		sunLight.direction = I4Vector3(-0.3f, -1.0f, -0.85f);
		sunLight.color = I4Vector3(1.0f, 1.0f, 1.0f);
		commit(&sunLight);
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

		if (I4ShaderMgr::addShaderMgr("shader/line.fx") == false)
		{
			I4LOG_ERROR << L"shader line add failed.";
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
		if (rtShadow->createDepthStencil(shadowSplitSize*SHADOW_SPLIT_NUM, shadowSplitSize, I4FORMAT_R32_TYPELESS, I4FORMAT_D32_FLOAT, I4FORMAT_R32_FLOAT) == false)
		{
			I4LOG_ERROR << L"render target shadow create failed.";
			return false;
		}

		// light mesh
		screenQuadMesh = new I4ScreenQuadMesh;
		if (screenQuadMesh->create() == false)
		{
			I4LOG_ERROR << L"quad mesh create failed.";
			return false;
		}

		pointLightMesh = new I4PointLightMesh;
		if (pointLightMesh->create(1.0f, 16, 16) == false)
		{
			I4LOG_ERROR << L"sphere mesh create failed.";
			return false;
		}
		

		if (cbOnResize_G.create() == false)
			return false;

		if (cbEveryFrame_G.create() == false)
			return false;

		if (cbEachAllMesh_G_VS.create() == false)
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

		if (cbEachFrame_Line.create() == false)
			return false;

		debugLineBatch = new I4LineBatch;
		debugLineBatch->create(1024);

		return true;
	}

	void I4DeferredRenderer::finalize()
	{
		delete debugLineBatch;
		delete pointLightMesh;
		delete screenQuadMesh;
		delete rtShadow;
		delete rtLight;
		delete rtDepth;
		delete rtNormal;
		delete rtSpecular;
		delete rtDiffuse;

		I4ShaderMgr::destroyShaderMgr();
		I4VideoDriver::destroyVideoDriver();
	}

	void I4DeferredRenderer::commit(const I4MeshRenderItem& item)
	{
		vecSceneMeshRenderItem.push_back(item);
	}

	void I4DeferredRenderer::commit(I4DirectionalLight* light)
	{
		sunLight = *light;

		directionalLightPerspectiveCamera.setLookAt(sunLight.direction*-100.0f, sunLight.direction, I4VECTOR3_AXISY);
		directionalLightPerspectiveCamera.setPerspectiveFov(I4PI/4, 1.0f, 0.1f, 100.0f);
	}

	void I4DeferredRenderer::commit(I4PointLight* light)
	{
		vecScenePointLight.push_back(*light);
	}

	void I4DeferredRenderer::commit(const I4DebugLine& line)
	{
		if (debugMode)
		{
			debugLineBatch->add(line.p0, line.p1, line.color);
		}
	}

	void I4DeferredRenderer::render()
	{
		I4PROFILE_THISFUNC;
		
		beginRender();

		renderStageGeometry();
		renderStageShadow();
		renderStageLight();
		renderStageMerge();
		renderDebug();
		
		endRender();

	}
	
	void I4DeferredRenderer::beginRender()
	{
		I4PROFILE_THISFUNC;

		videoDriver->beginScene();

		clearAllRenderTarget();
	}

	void I4DeferredRenderer::endRender()
	{
		I4PROFILE_THISFUNC;

		videoDriver->endScene();

		vecSceneMeshRenderItem.clear();
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

	void I4DeferredRenderer::renderStageGeometry()
	{
		I4PROFILE_THISFUNC;

		I4RenderTarget*	renderTargetG[] = { rtDiffuse, rtSpecular, rtNormal, rtDepth, rtLight };
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		cullAndSortMeshGeometryRenderItem();
		renderMeshGeometryRenderItem();		
	}

	void I4DeferredRenderer::cullAndSortMeshGeometryRenderItem()
	{
		I4PROFILE_THISFUNC;

		vecCulledMeshRenderItem.clear();

		for (auto& itr : vecSceneMeshRenderItem)
		{
			if (mainCamera.isVisibleAABB(itr.worldAABB))
			{
				vecCulledMeshRenderItem.push_back(itr);
			}
		}

		sort(vecCulledMeshRenderItem.begin(), vecCulledMeshRenderItem.end());
	}

	
	void I4DeferredRenderer::renderMeshGeometryRenderItem()
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

				cbOnResize_G.getData()->projection = mainCamera.getProjectionMatrix(); 
				cbOnResize_G.getData()->farDistance = mainCamera.getZFar();
				shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 0, cbOnResize_G.getBuffer(), cbOnResize_G.getData());

				cbEveryFrame_G.getData()->view = mainCamera.getViewMatrix();
				shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 1, cbEveryFrame_G.getBuffer(), cbEveryFrame_G.getData());
			}
		
			if (isChangedMesh)
			{
				if (prevItem != nullptr)
				{
					prevItem->mesh->unbind();
				}
					
				itr.mesh->bind();
				
				cbEachMeshInstance_G_PS.getData()->ambient = itr.material->ambient;
				cbEachMeshInstance_G_PS.getData()->specularGlossiness = itr.material->specularGlossiness;
				cbEachMeshInstance_G_PS.getData()->specularPower = itr.material->specularPower;
				shaderMgr->setConstantBuffer(I4SHADER_TYPE_PS, 3, cbEachMeshInstance_G_PS.getBuffer(), cbEachMeshInstance_G_PS.getData());				

				if (itr.mesh->skined)
				{
					buildMatrixPalette(cbEachSkinedMesh_G.getData()->matrixPalette, itr.resultTM, itr.skinTMs, itr.boneCount);
					shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 4, cbEachSkinedMesh_G.getBuffer(), cbEachSkinedMesh_G.getData());
				}
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

			cbEachAllMesh_G_VS.getData()->world = itr.worldTM;
			cbEachAllMesh_G_VS.getData()->result = itr.resultTM;
			shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 2, cbEachAllMesh_G_VS.getBuffer(), cbEachAllMesh_G_VS.getData());

			itr.mesh->draw();

			prevItem = &itr;
		}
				
		if (prevItem != nullptr)
		{
			prevItem->mesh->unbind();
		}

		shaderMgr->end();
	}

	void I4DeferredRenderer::renderStageShadow()
	{
		I4PROFILE_THISFUNC;

		I4RenderTarget*	renderTargetG[] = { rtShadow };
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG, rtShadow);
		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_NONE);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);		

		I4Camera tempSplitCamera;
		tempSplitCamera.setViewMatrix(mainCamera.getViewMatrix());

		I4Matrix4x4 matInvView;
		tempSplitCamera.getViewMatrix().extractInversePrimitive(matInvView);

		I4Matrix4x4 matToLightView = matInvView*directionalLightPerspectiveCamera.getViewMatrix();

		for (int i = 0; i < SHADOW_SPLIT_NUM; ++i)
		{
			directionalLightSplitOrthoCamera[i].setViewMatrix(directionalLightPerspectiveCamera.getViewMatrix());

			videoDriver->setViewport(i*shadowSplitSize, 0, shadowSplitSize, shadowSplitSize);
			
			float nearZ = mainCamera.getZNear();
			float farZ = mainCamera.getZFar();
			
			if (i != 0)
			{
				nearZ = shadowSplitZ[i - 1];
			}

			if (i != SHADOW_SPLIT_NUM - 1)
			{
				farZ = shadowSplitZ[i];
			}
			tempSplitCamera.setPerspectiveFov(mainCamera.getFovY(), mainCamera.getAspect(), nearZ, farZ);
																											
			I4Vector3 corners[8];
			tempSplitCamera.extractCorners(corners);

			for (int j = 0; j < 8; ++j)
			{
				corners[j] = matToLightView.transformCoord(corners[j]);				
			}

			I4AABB aabbInLightSpace;
			for (int j = 0; j < 8; ++j)
			{
				aabbInLightSpace.merge(corners[j]);
			}

			float minx = aabbInLightSpace.minEdge.x;
			float maxx = aabbInLightSpace.maxEdge.x;
			float miny = aabbInLightSpace.minEdge.y;
			float maxy = aabbInLightSpace.maxEdge.y;
			float minz = 0;
			float maxz = aabbInLightSpace.maxEdge.z;
			directionalLightSplitOrthoCamera[i].setOrthoOffCenter(minx, maxx, miny, maxy, minz, maxz);
			
			float finalMinX = FLT_MAX;
			float finalMaxX = -FLT_MAX;
			float finalMinY = FLT_MAX;
			float finalMaxY = -FLT_MAX;
			float finalMinZ = FLT_MAX;
			float finalMaxZ = -FLT_MAX;

			for (auto& itr : vecSceneMeshRenderItem)
			{
				if (itr.shadowCaster == true || itr.shadowReceiver == true)
				{
					if (directionalLightSplitOrthoCamera[i].isVisibleAABB(itr.worldAABB) == true)
					{
						I4AABB aabb = itr.worldAABB.transform(directionalLightSplitOrthoCamera[i].getViewMatrix());
						if (aabb.minEdge.x < finalMinX)
						{
							finalMinX = aabb.minEdge.x;
						}

						if (aabb.maxEdge.x > finalMaxX)
						{
							finalMaxX = aabb.maxEdge.x;
						}

						if (aabb.minEdge.y < finalMinY)
						{
							finalMinY = aabb.minEdge.y;
						}

						if (aabb.maxEdge.y > finalMaxY)
						{
							finalMaxY = aabb.maxEdge.y;
						}

						if (aabb.minEdge.z < finalMinZ)
						{
							finalMinZ = aabb.minEdge.z;
						}

						if (aabb.maxEdge.z > finalMaxZ)
						{
							finalMaxZ = aabb.maxEdge.z;
						}						
					}					
				}
			}

			minx = max(minx, finalMinX);
			maxx = min(maxx, finalMaxX);
			miny = max(miny, finalMinY);
			maxy = min(maxy, finalMaxY);
			minz = max(minz, finalMinZ);
			maxz = min(maxz, finalMaxZ);
			directionalLightSplitOrthoCamera[i].setOrthoOffCenter(minx, maxx, miny, maxy, minz, maxz);
			
			cullAndSortMeshShadowRenderItem(directionalLightSplitOrthoCamera[i]);
			renderMeshShadowRenderItem(directionalLightSplitOrthoCamera[i]);		
		}

		videoDriver->resetViewport();
	}

	void I4DeferredRenderer::cullAndSortMeshShadowRenderItem(const I4Camera& camera)
	{
		I4PROFILE_THISFUNC;

		vecCulledMeshRenderItem.clear();

		for (auto& itr : vecSceneMeshRenderItem)
		{
			if (itr.shadowCaster == true && camera.isVisibleAABB(itr.worldAABB) == true)
			{
				vecCulledMeshRenderItem.push_back(itr);
			}
		}

		sort(vecCulledMeshRenderItem.begin(), vecCulledMeshRenderItem.end());
	}

	void I4DeferredRenderer::renderMeshShadowRenderItem(const I4Camera& camera)
	{
		I4PROFILE_THISFUNC;

		I4MeshRenderItem* prevItem = nullptr;
		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/shadowmap.fx");

		for (auto& itr : vecCulledMeshRenderItem)
		{
			bool isChangedShader = false;
			bool isChangedMesh = false;
			bool isChangedTwoSide = false;

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

				if (itr.material->twoSide != prevItem->material->twoSide)
				{
					isChangedTwoSide = true;
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

				if (itr.mesh->skined)
				{
					buildMatrixPalette(cbEachSkinedMesh_S_VS.getData()->matrixPalette, itr.resultTM, itr.skinTMs, itr.boneCount);
					shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 1, cbEachSkinedMesh_S_VS.getBuffer(), cbEachSkinedMesh_S_VS.getData());
				}
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

			cbEachAllMesh_S_VS.getData()->worldViewProj = itr.worldTM*camera.getViewProjectionMatrix(); 
			shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 0, cbEachAllMesh_S_VS.getBuffer(), cbEachAllMesh_S_VS.getData());			
			
			itr.mesh->draw();

			prevItem = &itr;
		}
				
		if (prevItem != nullptr)
		{
			prevItem->mesh->unbind();
		}

		shaderMgr->end();
	}

	void I4DeferredRenderer::buildMatrixPalette(I4Matrix4x4* matrixPalette, const I4Matrix4x4& resultTM, const I4Matrix4x4* skinTMs, unsigned int boneCount)
	{
		// 매트릭스 팔레트를 만들어서 넘겨준다.
		// 또 모델이 바낄때마다 매트릭스 팔레트를 바꺼주고 resultTM을 셰이더에 넘겨주는것도 생각해봤는데 셰이더 연산량이 너무 많아서 안되겠다.
		// TODO : 지금은 모델 참조하고 있는 전체 본을 넘겨주는데 메시가 참조하고 있는 본만 가져와서 인덱스를 재구성해서 넘겨주는 식으로 바꺼주자.
		// 그럴려면 익스포터나 임포터 과정에서 전처리 해줘야할듯.
		for (unsigned int i = 0; i < boneCount; ++i)
		{
			
			matrixPalette[i] = resultTM*skinTMs[i];
		}
	}

	void I4DeferredRenderer::renderStageLight()
	{
		I4PROFILE_THISFUNC;

		videoDriver->setRenderTarget(1, &rtLight, nullptr);
		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);
		videoDriver->setBlendMode(I4BLEND_MODE_ADD);

		renderDirectionalLight();

		cullAndSortPointLight();
		renderPointLight();
	}

	void I4DeferredRenderer::renderDirectionalLight()
	{
		I4PROFILE_THISFUNC;

		shaderMgr = I4ShaderMgr::findShaderMgr("shader/deferred_l_directional.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));	
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);
		shaderMgr->setSamplerState(1, I4SAMPLER_STATE_POINT);
		shaderMgr->setSamplerState(2, I4SAMPLER_STATE_SHADOW);

		shaderMgr->setRenderTarget(0, rtNormal);
		shaderMgr->setRenderTarget(1, rtDepth);
		shaderMgr->setRenderTarget(2, rtShadow);

		I4Matrix4x4 matInvView;
		mainCamera.getViewMatrix().extractInversePrimitive(matInvView);
		for (int i = 0; i < SHADOW_SPLIT_NUM; ++i)
		{
			cbOnResize_L_directional.getData()->viewInvLightViewProjection[i] = matInvView*directionalLightSplitOrthoCamera[i].getViewProjectionMatrix();
			cbOnResize_L_directional.getData()->shadowSplitZ[i] = shadowSplitZ[i];
			cbOnResize_L_directional.getData()->shadowBias[i] = shadowBias[i];
		}
		cbOnResize_L_directional.getData()->shadowSplitSize = (float)shadowSplitSize;
		
		const I4Vector3 lightViewDir = mainCamera.getViewMatrix().transformVector(sunLight.direction);

		cbOnResize_L_directional.getData()->farTopRight = mainCamera.getFarTopRight();
		cbOnResize_L_directional.getData()->lightViewDirection = lightViewDir;
		cbOnResize_L_directional.getData()->lightColor = sunLight.color;

		shaderMgr->setConstantBuffer(I4SHADER_TYPE_PS, 0, cbOnResize_L_directional.getBuffer(), cbOnResize_L_directional.getData());

		screenQuadMesh->bind();
		screenQuadMesh->draw();
		screenQuadMesh->unbind();

		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);
		shaderMgr->setRenderTarget(2, nullptr);

		shaderMgr->end();
	}

	void I4DeferredRenderer::cullAndSortPointLight()
	{
		I4PROFILE_THISFUNC;

		vecCulledPointLight.clear();
		
		for (auto& itr : vecScenePointLight)
		{
			if (mainCamera.isVisibleSphere(itr.position, itr.radius))
			{
				vecCulledPointLight.push_back(itr);
			}
		}
	}

	void I4DeferredRenderer::renderPointLight()
	{
		I4PROFILE_THISFUNC;

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deferred_l_point.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS, _countof(I4INPUT_ELEMENTS_POS));
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);
		shaderMgr->setSamplerState(1, I4SAMPLER_STATE_POINT);

		shaderMgr->setRenderTarget(0, rtNormal);
		shaderMgr->setRenderTarget(1, rtDepth);
		
		cbOnResize_L_point_VS.getData()->projection = mainCamera.getProjectionMatrix();
		shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 0, cbOnResize_L_point_VS.getBuffer(), cbOnResize_L_point_VS.getData());

		cbOnResize_L_point_PS.getData()->farTopRight = mainCamera.getFarTopRight();
		shaderMgr->setConstantBuffer(I4SHADER_TYPE_PS, 1, cbOnResize_L_point_PS.getBuffer(), cbOnResize_L_point_PS.getData());

		cbEveryFrame_L_point.getData()->view = mainCamera.getViewMatrix();
		shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 2, cbEveryFrame_L_point.getBuffer(), cbEveryFrame_L_point.getData());

		pointLightMesh->bind();

		I4Matrix4x4 matLight;

		for (auto& itr : vecCulledPointLight)
		{
			const I4PointLight& light = itr;

			matLight.makeScale(light.radius, light.radius, light.radius);
			matLight.setPosition(light.position);

			
			const I4Vector3 lightViewPos =  mainCamera.getViewMatrix().transformCoord(light.position);

			// 카메라와 라이트의 관계를 찾는데 near plane 을 고려해서 생각해야한다.
			// 안그러면 카메라가 라이트의 외부에 있지만 near plane 안쪽에 있을때 깜박이는 현상이 생긴다.
			float lightRadiusPlusZNear = light.radius + mainCamera.getZNear();
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
			shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 3, cbEachLight_L_point_VS.getBuffer(), cbEachLight_L_point_VS.getData());


			cbEachLight_L_point_PS.getData()->lightPosition = lightViewPos;
			cbEachLight_L_point_PS.getData()->lightRadius = light.radius;
			cbEachLight_L_point_PS.getData()->lightColor = light.color;
			shaderMgr->setConstantBuffer(I4SHADER_TYPE_PS, 4, cbEachLight_L_point_PS.getBuffer(), cbEachLight_L_point_PS.getData());

			pointLightMesh->draw();
		}
		pointLightMesh->unbind();
		
		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);
		
		shaderMgr->end();
	}

	void I4DeferredRenderer::renderStageMerge()
	{
		I4PROFILE_THISFUNC;

		videoDriver->resetBackBufferRenderTarget(false);
		videoDriver->setRasterizerMode(I4RASTERIZER_MODE_SOLID_FRONT);
		videoDriver->setBlendMode(I4BLEND_MODE_NONE);

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/deferred_m.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));
		shaderMgr->setSamplerState(0, I4SAMPLER_STATE_LINEAR);

		shaderMgr->setRenderTarget(0, rtDiffuse);
		shaderMgr->setRenderTarget(1, rtSpecular);
		shaderMgr->setRenderTarget(2, rtLight);
		
		screenQuadMesh->bind();
		screenQuadMesh->draw();
		screenQuadMesh->unbind();
		
		shaderMgr->setRenderTarget(0, nullptr);
		shaderMgr->setRenderTarget(1, nullptr);
		shaderMgr->setRenderTarget(2, nullptr);

		shaderMgr->end();
	}

	void I4DeferredRenderer::renderDebug()
	{
		if (debugMode)
		{
			renderDebugLine();
		}
	}

	void I4DeferredRenderer::renderDebugLine()
	{
		I4PROFILE_THISFUNC;

		videoDriver->resetBackBufferRenderTarget(true);

//		videoDriver->setDepthStencilMode(I4DEPTH_OFF_STENCIL_OFF);


		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("shader/line.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_COL, _countof(I4INPUT_ELEMENTS_POS_COL));

		cbEachFrame_Line.getData()->viewProjection = mainCamera.getViewProjectionMatrix();
		shaderMgr->setConstantBuffer(I4SHADER_TYPE_VS, 0, cbEachFrame_Line.getBuffer(), cbEachFrame_Line.getData());

		debugLineBatch->draw();

		shaderMgr->end();

//		videoDriver->setDepthStencilMode(I4DEPTH_LESS_STENCIL_OFF);
	}

}