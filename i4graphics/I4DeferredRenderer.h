#pragma once

#include "I4Renderer.h"
#include "I4GeometryBuffer.h"
#include "I4Camera.h"
using namespace i4core;

namespace i4graphics
{
	const int SHADOW_SPLIT_NUM = 4;

	class VideoDriver;
	class ShaderMgr;
	class ModelMgr;
	class RenderTarget;
	class ScreenQuadMesh;
	class PointLightMesh;
	class Model;
	class ModelMgr;
	class TriangleMesh;
	class LineBatch;
	struct Material;

	__declspec(align(16))
	struct CBOnResize_G
	{
		Matrix4x4 projection;
		float farDistance;
	};

	__declspec(align(16))
	struct CBEveryFrame_G
	{
		Matrix4x4 view;
	};

	__declspec(align(16))
	struct CBEachAllMesh_G_VS
	{
		Matrix4x4 world;
		Matrix4x4	result;
	};

	__declspec(align(16))
	struct CBEachAllMesh_G_PS
	{
		float ambient;
		float specularLevel;
		float specularPower;

	};

	__declspec(align(16))
	struct CBEachSkinedMesh_G
	{
		Matrix4x4 matrixPalette[128];
	};

	__declspec(align(16))
	struct CBEachAllMesh_S_VS
	{
		Matrix4x4 worldViewProj;
	};

	__declspec(align(16))
	struct CBEachSkinedMesh_S_VS
	{
		Matrix4x4 matrixPalette[128];
	};

	__declspec(align(16))
	struct CBOnResize_L_directional
	{
		Matrix4x4 viewInvLightViewProjection[SHADOW_SPLIT_NUM];
		float shadowSplitZ[SHADOW_SPLIT_NUM];
		float shadowBias[SHADOW_SPLIT_NUM];
		Vector3 farTopRight;
		float shadowSplitSize;
		Vector3 lightViewDirection;
		float padding0;
		Vector3 lightColor;
		float padding1;
	};
	
	__declspec(align(16))
	struct CBOnResize_L_point_VS
	{
		Matrix4x4 projection;
	};

	__declspec(align(16))
	struct CBOnResize_L_point_PS
	{
		Vector3 farTopRight;
	};
	
	__declspec(align(16))
	struct CBEveryFrame_L_point
	{
		Matrix4x4 view;
	};

	__declspec(align(16))
	struct CBEachLight_L_point_VS
	{
		Matrix4x4 world;
	};

	__declspec(align(16))
	struct CBEachLight_L_point_PS
	{
		Vector3	lightPosition;
		float		lightRadius;
		Vector3	lightColor;
	};

	__declspec(align(16))
	struct CBEachFrame_Line
	{
		Matrix4x4 viewProjection;
	};

	template <typename T>
	class CBHolder
	{
	public:
		CBHolder()
			: buffer(nullptr)
			, data(nullptr)
		{
		}

		~CBHolder()
		{
			delete data;
			delete buffer;
		}

		bool create()
		{			
			buffer = VideoDriver::getVideoDriver()->createConstantBuffer();
			if (buffer->create(sizeof(T)) == false)
			{
				delete buffer;
				return false;
			}

			data = new T;

			return true;
		}

		ConstantBuffer*	getBuffer()		{ return buffer; }
		T*					getData()		{ return data; }

	private:
		ConstantBuffer*	buffer;	
		T*					data;
	};

	class DeferredRenderer : public Renderer
	{
		typedef vector<MeshRenderItem>		MeshRenderItemVector;
		typedef vector<DirectionalLight>		DirectionalLightVector;
		typedef vector<PointLight>			PointLightVector;
	public:
		DeferredRenderer();
		virtual ~DeferredRenderer(void);

		virtual bool		initialize(void* _windowID, unsigned int _width, unsigned int _height) override;
		void				finalize();

		virtual void		commit(const MeshRenderItem& item) override;
		virtual void		commit(DirectionalLight* light) override;
		virtual void		commit(PointLight* light) override;
		virtual void		commit(const DebugLine& line) override;
		virtual void		commit(const DebugBox& box) override;

		virtual void		render() override;

		void				renderDebug();

		void				beginRender();
		void				endRender();

		bool				isWireMode() const				{ return wireMode; }
		void				setWireMode(bool enable)		{ wireMode = enable; }

	private:
		void				clearAllRenderTarget();

		void				renderStageShadow();
		void				renderStageGeometry();
		void				renderStageLight();
		void				renderStageMerge();
		void				renderDebugLine();

		void				cullAndSortMeshGeometryRenderItem();
		void				renderMeshGeometryRenderItem();

		void				cullAndSortMeshShadowRenderItem(const Camera& camera);
		void				renderMeshShadowRenderItem(const Camera& camera);

		void				buildMatrixPalette(Matrix4x4* matrixPalette, const Matrix4x4& resultTM, const Matrix4x4* skinTMs, const int* boneRefTable, unsigned int boneCount);

		void				renderDirectionalLight();

		void				cullAndSortPointLight();
		void				renderPointLight();

	private:
		VideoDriver*			videoDriver;
		ShaderMgr*				shaderMgr;
		ModelMgr*				modelMgr;
		RenderTarget*			rtDiffuse;
		RenderTarget*			rtSpecular;
		RenderTarget*			rtNormal;
		RenderTarget*			rtDepth;
		RenderTarget*			rtLight;
		RenderTarget*			rtShadow;

		ScreenQuadMesh*			screenQuadMesh;
		PointLightMesh*			pointLightMesh;

		MeshRenderItemVector	vecSceneMeshRenderItem;
		PointLightVector		vecScenePointLight;

		MeshRenderItemVector	vecCulledMeshRenderItem;
		PointLightVector		vecCulledPointLight;

		DirectionalLight		sunLight;

		CBHolder<CBOnResize_G>				cbOnResize_G;
		CBHolder<CBEveryFrame_G>			cbEveryFrame_G;
		CBHolder<CBEachAllMesh_G_VS>		cbEachAllMesh_G_VS;
		CBHolder<CBEachAllMesh_G_PS>		cbEachMeshInstance_G_PS;
		CBHolder<CBEachSkinedMesh_G>		cbEachSkinedMesh_G;

		CBHolder<CBEachAllMesh_S_VS>		cbEachAllMesh_S_VS;
		CBHolder<CBEachSkinedMesh_S_VS>		cbEachSkinedMesh_S_VS;
		
		CBHolder<CBOnResize_L_directional>	cbOnResize_L_directional;

		CBHolder<CBOnResize_L_point_VS>		cbOnResize_L_point_VS;
		CBHolder<CBOnResize_L_point_PS>		cbOnResize_L_point_PS;
		CBHolder<CBEveryFrame_L_point>		cbEveryFrame_L_point;
		CBHolder<CBEachLight_L_point_VS>	cbEachLight_L_point_VS;
		CBHolder<CBEachLight_L_point_PS>	cbEachLight_L_point_PS;

		int			shadowSplitSize;
		float		shadowSplitZ[SHADOW_SPLIT_NUM];
		float		shadowBias[SHADOW_SPLIT_NUM];
		
		Camera	directionalLightPerspectiveCamera;
		Camera	directionalLightSplitOrthoCamera[4];

		LineBatch*						debugLineBatch;
		CBHolder<CBEachFrame_Line>		cbEachFrame_Line;
	};

}