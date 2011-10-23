#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"

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
		I4MeshInstance*		meshInstance;
	};

	class I4GRAPHICS_API I4DefferedRenderer
	{
		typedef std::list<I4MeshInstanceRenderItem>	I4MeshInstnaceRenderItemList;
	public:
		I4DefferedRenderer();
		~I4DefferedRenderer(void);

		bool				initialize(void* _windowID, unsigned int _width, unsigned int _height);
		void				finalize();

		I4ModelInstance*	createModelInstance(const char* modelPrototypeName, const char* modelInstanceName);
		void				destroyModelInstance(I4ModelInstance* modelInstance);

		void				commitModelInstance(I4ModelInstance* modelInstance);

		void				commitLight();

		void				preRender(I4Camera* camera);
		void				render(I4Camera* camera);
		void				postRender(I4Camera* camera);

	private:
		I4VideoDriver*					videoDriver;
		I4ShaderMgr*					shaderMgr;
		I4ModelMgr*						modelMgr;

		I4RenderTarget*					rtDiffuse;
		I4RenderTarget*					rtNormal;
		I4RenderTarget*					rtDepth;
		I4RenderTarget*					rtLight;

		I4QuadMesh*						quadMesh;
		I4SphereMesh*					sphereMesh;

		I4MeshInstnaceRenderItemList	listMeshInstnaceRenderItem;
	};

}