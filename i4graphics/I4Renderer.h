#pragma once

#include "I4AABB.h"
#include "I4Sphere.h"
using namespace i4core;

namespace i4core {
	class I4Camera;
}

namespace i4graphics {

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

	class I4GRAPHICS_API I4Renderer
	{
	public:
		I4Renderer()
			: wireMode(false)
		{
		}

		virtual ~I4Renderer(void)
		{
		}

		virtual bool	initialize(void* _windowID, unsigned int _width, unsigned int _height)	{ return true; }

		virtual void	commitToScene(const I4MeshRenderItem& item)		{}
		virtual void	commitToScene(I4DirectionalLight* light)		{}
		virtual void	commitToScene(I4PointLight* light)				{}

		virtual void	preRender(I4Camera* camera)						{}
		virtual void	render(I4Camera* camera)						{}
		virtual void	postRender(I4Camera* camera)					{}

		bool			isWireMode() const				{ return wireMode; }
		void			setWireMode(bool enable)		{ wireMode = enable; }

	protected:
		bool		wireMode;
	};
}