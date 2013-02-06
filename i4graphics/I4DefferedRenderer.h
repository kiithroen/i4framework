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

	#pragma pack(push, 16)	// 상수버퍼는 16바이트 정렬이 되어있어야한다.

	struct CBChageOnResize_G
	{
		I4Matrix4x4 projection;
		float farDistance;
	};

	struct CBChangesEveryFrame_G
	{
		I4Matrix4x4 view;
	};

	struct CBChangesEachMeshInstance_G
	{
		I4Matrix4x4 world;
		float specularIntensity;
		float specularPower;
	};

	struct CBChangeOnResize_L_directional
	{
		I4Vector3 farTopRight;
	};

	struct CBChangeEachLight_L_directional
	{
		I4Matrix4x4 lightViewProjection;
		I4Matrix4x4 viewInvLightViewProjection;
		I4Vector3 lightViewDirection;
		float		padding;	// 4바이트 얼라인을 안하니 셰이더에 넘어간 데이타가 꼬이는 현상이 생겼다.
		I4Vector3 lightColor;
	};

	struct CBChangeOnResize_L_point_VS
	{
		I4Matrix4x4 projection;
	};

	struct CBChangeOnResize_L_point_PS
	{
		I4Vector3 farTopRight;
	};
	
	struct CBChangeEveryFrame_L_point
	{
		I4Matrix4x4 view;
	};

	struct CBChangeEachLight_L_point_VS
	{
		I4Matrix4x4 world;
	};

	struct CBChangeEachLight_L_point_PS
	{
		I4Vector3	lightPosition;
		float		lightRadius;
		I4Vector3	lightColor;
	};

	struct ConstantBuffer
	{
		I4Matrix4x4 World;
		I4Matrix4x4 View;
		I4Matrix4x4 Projection;
	};

	#pragma pack(pop)

	class I4VertexBuffer;

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

		CBChageOnResize_G				cbChageOnResize_G;
		CBChangesEveryFrame_G			cbChangesEveryFrame_G;
		CBChangesEachMeshInstance_G		cbChangesEachMeshInstance_G;

		CBChangeOnResize_L_directional	cbChangeOnResize_L_directional;
		CBChangeEachLight_L_directional	cbChangeEachLight_L_directional;

		CBChangeOnResize_L_point_VS		cbChangeOnResize_L_point_VS;
		CBChangeOnResize_L_point_PS		cbChangeOnResize_L_point_PS;
		CBChangeEveryFrame_L_point		cbChangeEveryFrame_L_point;
		CBChangeEachLight_L_point_VS	cbChangeEachLight_L_point_VS;
		CBChangeEachLight_L_point_PS	cbChangeEachLight_L_point_PS;

		ConstantBuffer cb;
	};

}