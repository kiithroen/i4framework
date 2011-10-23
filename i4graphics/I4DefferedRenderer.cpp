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
	bool I4MeshInstanceRenderItem::operator < (I4MeshInstanceRenderItem& other) const
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
		I4VideoDriver::createVideoDriver(I4VIDEO_DRIVER_MODE_D3D10);
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
			I4LOG_ERROR << L"shader manager add failed.";
			return false;
		}
	
		if (I4ShaderMgr::addShaderMgr("deffered_l_directional.fx") == false)
		{
			I4LOG_ERROR << L"shader manager add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("deffered_l_point.fx") == false)
		{
			I4LOG_ERROR << L"shader manager add failed.";
			return false;
		}

		if (I4ShaderMgr::addShaderMgr("deffered_m.fx") == false)
		{
			I4LOG_ERROR << L"shader manager add failed.";
			return false;
		}
		
		// model mgr
		modelMgr = new I4ModelMgr;

		// render target
		rtDiffuse = videoDriver->createRenderTarget();
		if (rtDiffuse->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target create failed.";
			return false;
		}

		rtNormal = videoDriver->createRenderTarget();
		if (rtNormal->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target create failed.";
			return false;
		}

		rtDepth = videoDriver->createRenderTarget();
		if (rtDepth->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R32_FLOAT) == false)
		{
			I4LOG_ERROR << L"render target create failed.";
			return false;
		}

		rtLight = videoDriver->createRenderTarget();
		if (rtLight->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM) == false)
		{
			I4LOG_ERROR << L"render target create failed.";
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

	void I4DefferedRenderer::commitModelInstance(I4ModelInstance* modelInstance)
	{
		for (unsigned int i = 0; i < modelInstance->getSubCount(); ++i)
		{
			vecMeshInstnaceRenderItem.push_back(I4MeshInstanceRenderItem());
			I4MeshInstanceRenderItem& item = *vecMeshInstnaceRenderItem.rbegin();

			I4MeshInstance& meshInstance = modelInstance->getSubMeshInstance(i);			
			I4Matrix4x4::multiply(item.worldTM, meshInstance.meshLocalTM, modelInstance->getModelTM());
			item.meshInstance = &meshInstance;			
		}
	}

	void I4DefferedRenderer::preRender(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		std::sort(vecMeshInstnaceRenderItem.begin(), vecMeshInstnaceRenderItem.end());
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
		vecMeshInstnaceRenderItem.clear();;
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
		videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG, true);

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("deffered_g.fx");
		//shaderMgr->begin(I4SHADER_MASK_DIFFUSEMAP|I4SHADER_MASK_SPECULARMAP|I4SHADER_MASK_NORMALMAP, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));
				
		shaderMgr->setMatrix(I4SHADER_MATRIX_PROJECTION, camera->getProjectionMatrix().arr);
		shaderMgr->setFloat(I4SHADER_FLOAT_FAR_DISTANCE, camera->getZFar());

		shaderMgr->setMatrix(I4SHADER_MATRIX_VIEW, camera->getViewMatrix().arr);

		I4MeshInstance* prevMeshInstance = NULL;
		I4MeshInstance* curMeshInstance = NULL;
		I4StaticMesh* prevMesh = NULL;
		I4StaticMesh* curMesh = NULL;

		I4MeshInstnaceRenderItemVector::iterator itr = vecMeshInstnaceRenderItem.begin();
		const I4MeshInstnaceRenderItemVector::iterator itrEnd = vecMeshInstnaceRenderItem.end();
		for (; itr != itrEnd; ++itr)
		{
			curMeshInstance = itr->meshInstance;					
			bool isChangedDiffuseMap = false;
			bool isChangedSpecularMap = false;
			bool isChangedNormalMap = false;
			bool isChangedMesh = false;

			if (prevMeshInstance != curMeshInstance)
			{
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
					shaderMgr->setTexture(I4SHADER_TEXTURE_DIFFUSEMAP, diffuse);
				}
						
				if (isChangedSpecularMap == true)
				{
					I4Texture* specular = modelMgr->findTexture(curMeshInstance->specularMapID);
					shaderMgr->setTexture(I4SHADER_TEXTURE_SPECULARMAP, specular);
				}

				if (isChangedNormalMap == true)
				{
					I4Texture* normal = modelMgr->findTexture(curMeshInstance->normalMapID);
					shaderMgr->setTexture(I4SHADER_TEXTURE_NORMALMAP, normal);
				}

				shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_INTENSITY, curMeshInstance->specularInensity);
				shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_POWER, curMeshInstance->specularPower);		

				shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, itr->worldTM.arr);
				shaderMgr->apply();

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

		renderDirectionalLight(camera);
		renderPointLight(camera);
	}

	void I4DefferedRenderer::renderDirectionalLight(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("deffered_l_directional.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));			

		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, rtNormal);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, rtDepth);
				
		shaderMgr->setVector(I4SHADER_VECTOR_FAR_TOP_RIGHT, camera->getFarTopRight().xyz);
		shaderMgr->apply();

		I4Vector3 lightDirection[] =
		{					
			I4Vector3(-1.0f, -1.0f, 0.0f),
			I4Vector3(1.0f, 0.5f, 0.5f),
		};

		I4Vector3 lightColor[] =
		{										
			I4Vector3(1.0f, 1.0f, 1.0f),
			I4Vector3(0.6f, 0.725f, 1.0f)*0.9f,
		};

		quadMesh->bind();

		I4Matrix4x4 matLight;
		for (int i = 0; i < 2; ++i)
		{
			if (i == 0)
			{						
				matLight.makeIdentity();
			}
			else
			{
				static float lightAng = 0;
				lightAng += 0.2f;
				if (lightAng > 360)
				{
					lightAng -= 360;
				}

				matLight.makeRotationY(mathutil::degreeToRadian(lightAng));
			}

			I4Matrix4x4 matLightView;
			I4Matrix4x4::multiply(matLightView, matLight, camera->getViewMatrix());
			I4Vector3 lightDir = matLightView.transformVector(lightDirection[i]);
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_DIRECTION, lightDir.xyz);
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_COLOR, lightColor[i].xyz);			

			shaderMgr->apply();
			quadMesh->draw();
		}
		quadMesh->unbind();

		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, NULL);
		shaderMgr->apply();

		shaderMgr->end();
	}

	void I4DefferedRenderer::renderPointLight(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("deffered_l_point.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS, _countof(I4INPUT_ELEMENTS_POS));

		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, rtNormal);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, rtDepth);
		
		shaderMgr->setMatrix(I4SHADER_MATRIX_PROJECTION, camera->getProjectionMatrix().arr);
		shaderMgr->setMatrix(I4SHADER_MATRIX_VIEW, camera->getViewMatrix().arr);
		shaderMgr->setVector(I4SHADER_VECTOR_FAR_TOP_RIGHT, camera->getFarTopRight().xyz);
		shaderMgr->apply();

		I4Vector4 lightPointRadius[] =
		{
			I4Vector4(0.0f, 3.0f, -2.0f, 7.0f),
			I4Vector4(-1.0f, 3.0f, -5.0f, 7.0f),
			I4Vector4(3.5f, 6.0f, -4.5f, 7.0f),
			I4Vector4(1.0f, 5.5f, -7.0f, 7.0),
			I4Vector4(2.0f, 3.0f, -4.5f, 7.0),
		};

		I4Vector3 lightPointColor[] =
		{
			I4Vector3(1.0f, 0.125f, 0.93f),
			I4Vector3(1.0f, 0.0f, 0.0f),
			I4Vector3(0.0f, 0.8f, 0.8f),
			I4Vector3(1.0f, 1.0f, 0.0f),
			I4Vector3(1.0f, 0.5f, 0.25f),
		};

		sphereMesh->bind();
		for (int i = 0; i < 5; ++i)
		{
			I4Matrix4x4 matScale;
			matScale.makeScale(lightPointRadius[i].w, lightPointRadius[i].w, lightPointRadius[i].w);

					
			I4Matrix4x4 matRot;
			if (i%2 ==  0)
			{
				static float lightAng = 0;
				lightAng += 0.1f;
				matRot.makeRotationAxis(I4Vector3(0.0f, 0.2f, 1.0f), mathutil::degreeToRadian(lightAng));
				if (lightAng > 360)
				{
					lightAng -= 360;
				}
			}
			else
			{
				static float lightAng = 0;
				lightAng += 0.2f;
				matRot.makeRotationAxis(I4Vector3(1.0f, 0.5f, 0.5f), mathutil::degreeToRadian(-lightAng));
				if (lightAng > 360)
				{
					lightAng -= 360;
				}
			}

			I4Matrix4x4 matTrans;
			matTrans.makeTranslation(lightPointRadius[i].x, lightPointRadius[i].y, lightPointRadius[i].z);

			I4Matrix4x4 matLight = matScale*matTrans*matRot;	

			I4Vector3 lightWorldPos;
			matLight.extractTranslation(lightWorldPos);

			if (camera->isVisibleSphere(lightWorldPos, lightPointRadius[i].w) == false)
				continue;

			shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, matLight.arr);
					
			I4Matrix4x4 matLightView = camera->getViewMatrix();
			I4Vector3 lightViewPos = matLightView.transformCoord(lightWorldPos);
			I4Vector4 light;
			light.x = lightViewPos.x;
			light.y = lightViewPos.y;
			light.z = lightViewPos.z;
			light.w = lightPointRadius[i].w;

					
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_POINT_RADIUS, light.xyzw);
			shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_COLOR, lightPointColor[i].xyz);			

			shaderMgr->apply();
			sphereMesh->draw();
		}
		sphereMesh->unbind();

		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, NULL);
		shaderMgr->apply();

		shaderMgr->end();
	}

	void I4DefferedRenderer::renderStageMerge(I4Camera* camera)
	{
		PROFILE_THISFUNC;

		videoDriver->resetRenderTarget();

		I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr("deffered_m.fx");
		shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));

		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_LIGHT, rtLight);
			
		shaderMgr->apply();
		quadMesh->bind();
		quadMesh->draw();
		quadMesh->unbind();

		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
		shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_LIGHT, NULL);
		shaderMgr->apply();
	}
}