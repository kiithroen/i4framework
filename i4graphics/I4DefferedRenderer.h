#pragma once

#include "i4graphics.h"
#include "I4AABB.h"
#include "I4Sphere.h"
#include "I4GeometryBuffer.h"

namespace i4core
{	
	class I4Camera;
}
using namespace i4core;

namespace i4graphics
{
	class I4VideoDriver;
	class I4ShaderMgr;
	class I4ActorMgr;
	class I4RenderTarget;
	class I4QuadMesh;
	class I4SphereMesh;
	class I4ModelInstance;
	class I4MeshInstance;
	class I4Actor;
	class I4ActorMgr;
	class I4StaticMesh;

	struct I4MeshInstanceRenderItem
	{
		I4Matrix4x4			worldTM;
		I4AABB				worldAABB;
		I4StaticMesh*		mesh;

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

	__declspec(align(16))
	struct CBChageOnResize_G
	{
		I4Matrix4x4 projection;
		float farDistance;
	};

	__declspec(align(16))
	struct CBChangesEveryFrame_G
	{
		I4Matrix4x4 view;
	};

	__declspec(align(16))
	struct CBChangesEachMeshInstance_G
	{
		I4Matrix4x4 world;
		float specularIntensity;
		float specularPower;
	};

	__declspec(align(16))
	struct CBChangeOnResize_L_directional
	{
		I4Vector3 farTopRight;
	};

	__declspec(align(16))
	struct CBChangeEachLight_L_directional
	{
		I4Matrix4x4 lightViewProjection;
		I4Matrix4x4 viewInvLightViewProjection;
		I4Vector3 lightViewDirection;
		float		padding;	// 4바이트 얼라인을 안하니 셰이더에 넘어간 데이타가 꼬이는 현상이 생겼다.
		I4Vector3 lightColor;
	};

	__declspec(align(16))
	struct CBChangeOnResize_L_point_VS
	{
		I4Matrix4x4 projection;
	};

	__declspec(align(16))
	struct CBChangeOnResize_L_point_PS
	{
		I4Vector3 farTopRight;
	};
	
	__declspec(align(16))
	struct CBChangeEveryFrame_L_point
	{
		I4Matrix4x4 view;
	};

	__declspec(align(16))
	struct CBChangeEachLight_L_point_VS
	{
		I4Matrix4x4 world;
	};

	__declspec(align(16))
	struct CBChangeEachLight_L_point_PS
	{
		I4Vector3	lightPosition;
		float		lightRadius;
		I4Vector3	lightColor;
	};

	__declspec(align(16))
	struct ConstantBuffer
	{
		I4Matrix4x4 World;
		I4Matrix4x4 View;
		I4Matrix4x4 Projection;
	};
	
	template <typename T>
	class I4ConstantBufferHodler
	{
	public:
		I4ConstantBufferHodler()
			: buffer(nullptr)
			, data(nullptr)
		{
		}

		~I4ConstantBufferHodler()
		{
			delete data;
			delete buffer;
		}

		bool create()
		{			
			buffer = I4VideoDriver::getVideoDriver()->createConstantBuffer();
			if (buffer->create(sizeof(T)) == false)
			{
				delete buffer;
				return false;
			}

			data = new T;

			return true;
		}

		I4ConstantBuffer*	getBuffer()		{ return buffer; }
		T*					getData()		{ return data; }

	private:
		I4ConstantBuffer*	buffer;	
		T*					data;
	};

	class I4GRAPHICS_API I4DefferedRenderer
	{
		typedef vector<I4MeshInstanceRenderItem>	I4MeshInstnaceRenderItemVector;
		typedef vector<I4DirectionalLight>			I4DirectionalLightVector;
		typedef vector<I4PointLight>				I4PointLightVector;
	public:
		I4DefferedRenderer();
		~I4DefferedRenderer(void);

		bool				initialize(void* _windowID, unsigned int _width, unsigned int _height);
		void				finalize();

		void				commitToScene(const I4MeshInstanceRenderItem& item);
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
		I4ActorMgr*						actorMgr;
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

		I4ConstantBufferHodler<CBChageOnResize_G>				cbChageOnResize_G;
		I4ConstantBufferHodler<CBChangesEveryFrame_G>			cbChangesEveryFrame_G;
		I4ConstantBufferHodler<CBChangesEachMeshInstance_G>		cbChangesEachMeshInstance_G;

		I4ConstantBufferHodler<CBChangeOnResize_L_directional>	cbChangeOnResize_L_directional;
		I4ConstantBufferHodler<CBChangeEachLight_L_directional>	cbChangeEachLight_L_directional;

		I4ConstantBufferHodler<CBChangeOnResize_L_point_VS>		cbChangeOnResize_L_point_VS;
		I4ConstantBufferHodler<CBChangeOnResize_L_point_PS>		cbChangeOnResize_L_point_PS;
		I4ConstantBufferHodler<CBChangeEveryFrame_L_point>		cbChangeEveryFrame_L_point;
		I4ConstantBufferHodler<CBChangeEachLight_L_point_VS>	cbChangeEachLight_L_point_VS;
		I4ConstantBufferHodler<CBChangeEachLight_L_point_PS>	cbChangeEachLight_L_point_PS;

		ConstantBuffer cb;
	};

}