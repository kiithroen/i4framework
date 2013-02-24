#pragma once

#include "i4graphics.h"
#include "I4AABB.h"
#include "I4Sphere.h"
#include "I4GeometryBuffer.h"
#include "I4Camera.h"

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
	class I4Actor;
	class I4ActorMgr;
	class I4Mesh;
	struct I4Material;

	struct I4MeshRenderItem
	{
		I4Matrix4x4			worldTM;
		I4AABB				worldAABB;
		unsigned int		shaderMask;
		unsigned int		boneCount;
		I4Mesh*				mesh;
		I4Material*			material;
		I4Matrix4x4*		matrixPalette;
		bool				shadowCaster;
		bool				shadowReceiver;
				
		bool operator < (const I4MeshRenderItem& other) const;
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
	struct CBOnResize_G
	{
		I4Matrix4x4 projection;
		float farDistance;
	};

	__declspec(align(16))
	struct CBEveryFrame_G
	{
		I4Matrix4x4 view;
	};

	__declspec(align(16))
	struct CBEachAllMesh_G_VS
	{
		I4Matrix4x4 world;
	};

	__declspec(align(16))
	struct CBEachAllMesh_G_PS
	{
		float ambient;
		float specularGlossiness;
		float specularPower;

	};

	__declspec(align(16))
	struct CBEachSkinedMesh_G
	{
		I4Matrix4x4 matrixPalette[120];
	};

	__declspec(align(16))
	struct CBEachAllMesh_S_VS
	{
		I4Matrix4x4 worldViewProj;
	};

	__declspec(align(16))
	struct CBEachSkinedMesh_S_VS
	{
		I4Matrix4x4 matrixPalette[120];
	};

	__declspec(align(16))
	struct CBOnResize_L_directional
	{
		I4Vector3 farTopRight;
	};

	__declspec(align(16))
	struct CBEachLight_L_directional
	{
		I4Matrix4x4 viewInvLightViewProjection[3];
		I4Vector3 lightViewDirection;
		float		padding;	// 4바이트 얼라인을 안하니 셰이더에 넘어간 데이타가 꼬이는 현상이 생겼다.
		I4Vector3 lightColor;
	};

	__declspec(align(16))
	struct CBOnResize_L_point_VS
	{
		I4Matrix4x4 projection;
	};

	__declspec(align(16))
	struct CBOnResize_L_point_PS
	{
		I4Vector3 farTopRight;
	};
	
	__declspec(align(16))
	struct CBEveryFrame_L_point
	{
		I4Matrix4x4 view;
	};

	__declspec(align(16))
	struct CBEachLight_L_point_VS
	{
		I4Matrix4x4 world;
	};

	__declspec(align(16))
	struct CBEachLight_L_point_PS
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
	class I4CBHolder
	{
	public:
		I4CBHolder()
			: buffer(nullptr)
			, data(nullptr)
		{
		}

		~I4CBHolder()
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

	class I4GRAPHICS_API I4DeferredRenderer
	{
		typedef vector<I4MeshRenderItem>		I4MeshRenderItemVector;
		typedef vector<I4DirectionalLight>		I4DirectionalLightVector;
		typedef vector<I4PointLight>			I4PointLightVector;
	public:
		I4DeferredRenderer();
		~I4DeferredRenderer(void);

		bool				initialize(void* _windowID, unsigned int _width, unsigned int _height);
		void				finalize();

		void				commitToScene(const I4MeshRenderItem& item);
		void				commitToScene(I4DirectionalLight* light);
		void				commitToScene(I4PointLight* light);

		void				preRender(I4Camera* camera);
		void				render(I4Camera* camera);
		void				postRender(I4Camera* camera);

		bool				isWireMode() const				{ return wireMode; }
		void				setWireMode(bool enable)		{ wireMode = enable; }

	private:
		void				clearAllRenderTarget();

		void				renderStageShadow(I4Camera* camera);
		void				renderStageGeometry(I4Camera* camera);
		void				renderStageLight(I4Camera* camera);
		void				renderStageMerge(I4Camera* camera);

		void				cullAndSortMeshRenderItem(I4Camera* camera);
		void				renderMeshGeometryRenderItem(I4Camera* camera);
		void				renderMeshShadowRenderItem(I4Camera* camera);

		void				cullAndSortDirectionalLight(I4Camera* camera);
		void				renderDirectionalLight(I4Camera* camera);

		void				cullAndSortPointLight(I4Camera* camera);
		void				renderPointLight(I4Camera* camera);

	private:
		I4VideoDriver*					videoDriver;
		I4ShaderMgr*					shaderMgr;
		I4ActorMgr*						actorMgr;
		I4RenderTarget*					rtDiffuse;
		I4RenderTarget*					rtSpecular;
		I4RenderTarget*					rtNormal;
		I4RenderTarget*					rtDepth;
		I4RenderTarget*					rtLight;
		I4RenderTarget*					rtShadow;

		I4QuadMesh*						quadMesh;
		I4SphereMesh*					sphereMesh;

		I4MeshRenderItemVector			vecSceneMeshRenderItem;
		I4DirectionalLightVector		vecSceneDirectionalLight;
		I4PointLightVector				vecScenePointLight;

		I4MeshRenderItemVector			vecCulledMeshRenderItem;
		I4DirectionalLightVector		vecCulledDirectionalLight;
		I4PointLightVector				vecCulledPointLight;

		I4CBHolder<CBOnResize_G>				cbOnResize_G;
		I4CBHolder<CBEveryFrame_G>				cbEveryFrame_G;
		I4CBHolder<CBEachAllMesh_G_VS>			cbEachMeshInstance_G_VS;
		I4CBHolder<CBEachAllMesh_G_PS>			cbEachMeshInstance_G_PS;
		I4CBHolder<CBEachSkinedMesh_G>			cbEachSkinedMesh_G;

		I4CBHolder<CBEachAllMesh_S_VS>			cbEachAllMesh_S_VS;
		I4CBHolder<CBEachSkinedMesh_S_VS>		cbEachSkinedMesh_S_VS;
		
		I4CBHolder<CBOnResize_L_directional>	cbOnResize_L_directional;
		I4CBHolder<CBEachLight_L_directional>	cbEachLight_L_directional;

		I4CBHolder<CBOnResize_L_point_VS>		cbOnResize_L_point_VS;
		I4CBHolder<CBOnResize_L_point_PS>		cbOnResize_L_point_PS;
		I4CBHolder<CBEveryFrame_L_point>		cbEveryFrame_L_point;
		I4CBHolder<CBEachLight_L_point_VS>		cbEachLight_L_point_VS;
		I4CBHolder<CBEachLight_L_point_PS>		cbEachLight_L_point_PS;

		bool						wireMode;
		int							cascadeSize;
		int							cascadeLevel;
		
		I4Camera lightCam[3];
	};

}