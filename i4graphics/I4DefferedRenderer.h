#pragma once

#include "i4graphics.h"
#include "I4AABB.h"
#include "I4Sphere.h"

namespace i4core
{	
	class I4Camera;
}

namespace i4graphics
{
	class I4VideoDriver;
	class I4ShaderMgr;
	class I4ModelMgr;
	class I4RenderTarget;
	class I4QuadMesh;
	class I4SphereMesh;
	class I4ModelInstance;
	class I4MeshInstance;

	struct I4MeshInstanceRenderItem
	{
		I4Matrix4x4			worldTM;
		I4AABB				worldAABB;
		I4MeshInstance*		meshInstance;

		bool operator < (const I4MeshInstanceRenderItem& other) const;
	};


	struct I4DirectionalLight
	{
		I4Vector3	direction;
		I4Vector3	color;
	};

	struct I4PointLight
	{
		I4Vector3	position;
		float		radius;
		I4Vector3	color;
	};

	class I4GRAPHICS_API I4DefferedRenderer
	{
		typedef std::vector<I4MeshInstanceRenderItem>	I4MeshInstnaceRenderItemVector;
		typedef std::vector<I4DirectionalLight>			I4DirectionalLightVector;
		typedef std::vector<I4PointLight>				I4PointLightVector;
	public:
		I4DefferedRenderer();
		~I4DefferedRenderer(void);

		bool				initialize(void* _windowID, unsigned int _width, unsigned int _height);
		void				finalize();

		I4ModelInstance*	createModelInstance(const char* modelPrototypeName, const char* modelInstanceName);
		void				destroyModelInstance(I4ModelInstance* modelInstance);

		void				commitToScene(I4ModelInstance* modelInstance);
		void				commitToScene(I4DirectionalLight* light);
		void				commitToScene(I4PointLight* light);

		void				preRender(I4Camera* camera);
		void				render(I4Camera* camera);
		void				postRender(I4Camera* camera);

	private:
		void				clearAllRenderTarget();

		void				renderStageGeometry(I4Camera* camera);
		void				renderStageLight(I4Camera* camera);
		void				renderStageMerge(I4Camera* camera);

		void				cullAndSortMeshInstanceRenderItem(I4Camera* camera);
		void				renderMeshInstanceRenderItem(I4Camera* camera);

		void				cullAndSortDirectionalLight(I4Camera* camera);
		void				renderDirectionalLight(I4Camera* camera);

		void				cullAndSortPointLight(I4Camera* camera);
		void				renderPointLight(I4Camera* camera);

	private:
		I4VideoDriver*					videoDriver;
		I4ShaderMgr*					shaderMgr;
		I4ModelMgr*						modelMgr;

		I4RenderTarget*					rtDiffuse;
		I4RenderTarget*					rtNormal;
		I4RenderTarget*					rtDepth;
		I4RenderTarget*					rtLight;
		I4RenderTarget*					rtShadow;

		I4QuadMesh*						quadMesh;
		I4SphereMesh*					sphereMesh;

		I4MeshInstnaceRenderItemVector	vecSceneMeshInstnaceRenderItem;
		I4DirectionalLightVector		vecSceneDirectionalLight;
		I4PointLightVector				vecScenePointLight;

		I4MeshInstnaceRenderItemVector	vecCulledMeshInstnaceRenderItem;
		I4DirectionalLightVector		vecCulledDirectionalLight;
		I4PointLightVector				vecCulledPointLight;
	};

}