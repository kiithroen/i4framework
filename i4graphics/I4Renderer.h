#pragma once

#include "I4AABB.h"
#include "I4Sphere.h"
#include "I4Vector4.h"
#include "I4Camera.h"
using namespace i4core;

namespace i4graphics {

	class I4TriangleMesh;
	struct I4Material;

	struct I4MeshRenderItem
	{
		I4Matrix4x4			worldTM;
		I4AABB				worldAABB;
		unsigned int		shaderMask;
		unsigned int		boneCount;
		I4TriangleMesh*		mesh;
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

	struct I4DebugLine
	{
		I4Vector3	p0;
		I4Vector3	p1;
		I4Vector4	color;
	};

	class I4Renderer
	{
	public:
		I4Renderer()
			: wireMode(false)
			, debugMode(false)
			, mainCamera()
		{
		}

		virtual ~I4Renderer(void)
		{
		}

		virtual bool	initialize(void* _windowID, unsigned int _width, unsigned int _height)	{ return true; }

		virtual void	commit(const I4MeshRenderItem& item)			{}
		virtual void	commit(I4DirectionalLight* light)				{}
		virtual void	commit(I4PointLight* light)						{}
		virtual void	commit(const I4DebugLine& line)					{}

		virtual void	render()										{}

		I4Camera&		getMainCamera()					{ return mainCamera; }

		bool			isWireMode() const				{ return wireMode; }
		void			setWireMode(bool enable)		{ wireMode = enable; }

		bool			isDebugMode() const				{ return debugMode; }
		void			setDebugMode(bool enable)		{ debugMode = enable; }

	protected:
		I4Camera	mainCamera;
		bool		wireMode;
		bool		debugMode;
	};
}