#pragma once

#include "I4Renderer.h"
#include "I4GeometryBuffer.h"
#include "I4Camera.h"
using namespace i4core;

namespace i4graphics
{
	const int SHADOW_SPLIT_NUM = 4;

	class I4VideoDriver;
	class I4ShaderMgr;
	class I4ModelMgr;
	class I4RenderTarget;
	class I4ScreenQuadMesh;
	class I4PointLightMesh;
	class I4Model;
	class I4ModelMgr;
	class I4TriangleMesh;
	class I4LineMesh;
	struct I4Material;

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
		I4Matrix4x4 viewInvLightViewProjection[SHADOW_SPLIT_NUM];
		float shadowSplitZ[SHADOW_SPLIT_NUM];
		float shadowBias[SHADOW_SPLIT_NUM];
		I4Vector3 farTopRight;
		float shadowSplitSize;
		I4Vector3 lightViewDirection;
		float padding0;
		I4Vector3 lightColor;
		float padding1;
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
	
	__declspec(align(16))
	struct CBEachFrame_Line
	{
		I4Matrix4x4 viewProjection;
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

	class I4DeferredRenderer : public I4Renderer
	{
		typedef vector<I4MeshRenderItem>		I4MeshRenderItemVector;
		typedef vector<I4DirectionalLight>		I4DirectionalLightVector;
		typedef vector<I4PointLight>			I4PointLightVector;
	public:
		I4DeferredRenderer();
		virtual ~I4DeferredRenderer(void);

		virtual bool				initialize(void* _windowID, unsigned int _width, unsigned int _height) override;
		void						finalize();

		virtual void				commitToScene(const I4MeshRenderItem& item) override;
		virtual void				commitToScene(I4DirectionalLight* light) override;
		virtual void				commitToScene(I4PointLight* light) override;
		virtual void				commitToScene(const I4DebugLine& line) override;

		virtual void				preRender(I4Camera* camera) override;
		virtual void				render(I4Camera* camera) override;


		virtual void				postRender(I4Camera* camera) override;

		bool				isWireMode() const				{ return wireMode; }
		void				setWireMode(bool enable)		{ wireMode = enable; }

	private:
		void				clearAllRenderTarget();

		void				renderStageShadow(I4Camera* camera);
		void				renderStageGeometry(I4Camera* camera);
		void				renderStageLight(I4Camera* camera);
		void				renderStageMerge(I4Camera* camera);
		void				renderDebugLine(I4Camera* camera);

		void				cullAndSortMeshGeometryRenderItem(I4Camera* camera);
		void				renderMeshGeometryRenderItem(I4Camera* camera);
		void				cullAndSortMeshShadowRenderItem(I4Camera* camera);
		void				renderMeshShadowRenderItem(I4Camera* camera);

		void				renderDirectionalLight(I4Camera* camera);

		void				cullAndSortPointLight(I4Camera* camera);
		void				renderPointLight(I4Camera* camera);

	private:
		I4VideoDriver*					videoDriver;
		I4ShaderMgr*					shaderMgr;
		I4ModelMgr*						modelMgr;
		I4RenderTarget*					rtDiffuse;
		I4RenderTarget*					rtSpecular;
		I4RenderTarget*					rtNormal;
		I4RenderTarget*					rtDepth;
		I4RenderTarget*					rtLight;
		I4RenderTarget*					rtShadow;

		I4ScreenQuadMesh*				screenQuadMesh;
		I4PointLightMesh*				pointLightMesh;

		I4MeshRenderItemVector			vecSceneMeshRenderItem;
		I4PointLightVector				vecScenePointLight;

		I4MeshRenderItemVector			vecCulledMeshRenderItem;
		I4PointLightVector				vecCulledPointLight;

		I4DirectionalLight				sunLight;

		I4CBHolder<CBOnResize_G>				cbOnResize_G;
		I4CBHolder<CBEveryFrame_G>				cbEveryFrame_G;
		I4CBHolder<CBEachAllMesh_G_VS>			cbEachMeshInstance_G_VS;
		I4CBHolder<CBEachAllMesh_G_PS>			cbEachMeshInstance_G_PS;
		I4CBHolder<CBEachSkinedMesh_G>			cbEachSkinedMesh_G;

		I4CBHolder<CBEachAllMesh_S_VS>			cbEachAllMesh_S_VS;
		I4CBHolder<CBEachSkinedMesh_S_VS>		cbEachSkinedMesh_S_VS;
		
		I4CBHolder<CBOnResize_L_directional>	cbOnResize_L_directional;

		I4CBHolder<CBOnResize_L_point_VS>		cbOnResize_L_point_VS;
		I4CBHolder<CBOnResize_L_point_PS>		cbOnResize_L_point_PS;
		I4CBHolder<CBEveryFrame_L_point>		cbEveryFrame_L_point;
		I4CBHolder<CBEachLight_L_point_VS>		cbEachLight_L_point_VS;
		I4CBHolder<CBEachLight_L_point_PS>		cbEachLight_L_point_PS;

		int			shadowSplitSize;
		float		shadowSplitZ[SHADOW_SPLIT_NUM];
		float		shadowBias[SHADOW_SPLIT_NUM];
		
		I4Camera	directionalLightPerspectiveCamera;
		I4Camera	directionalLightSplitOrthoCamera[4];

		list<I4DebugLine>					listDebugLine;
		I4LineMesh*							lineDebugMesh;
		I4CBHolder<CBEachFrame_Line>		cbEachFrame_Line;
	};

}