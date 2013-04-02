#pragma once

#include "I4AABB.h"
#include "I4Sphere.h"
#include "I4Vector4.h"
#include "I4Camera.h"
using namespace i4core;

namespace i4graphics {

	class Model;
	class TriangleMesh;
	struct Material;

	struct MeshRenderItem
	{
		Matrix4x4			worldTM;
		Matrix4x4			resultTM;
		AABB				worldAABB;
		unsigned int		shaderMask;
		unsigned int		boneCount;
		unsigned int		subMeshID;
		TriangleMesh*		mesh;
		Material*			material;
		Matrix4x4*		skinTMs;
		bool				shadowCaster;
		bool				shadowReceiver;

		bool operator < (const MeshRenderItem& other) const;
	};


	struct DirectionalLight
	{
		Vector3	direction;
		Vector3	color;
	};

	struct PointLight
	{
		Vector3	position;
		float		radius;
		Vector3	color;
	};

	struct DebugLine
	{
		Vector3	p0;
		Vector3	p1;
		Vector4	color;
	};

	class Renderer
	{
	public:
		Renderer()
			: wireMode(false)
			, debugMode(false)
			, mainCamera()
		{
		}

		virtual ~Renderer(void)
		{
		}

		virtual bool	initialize(void* _windowID, unsigned int _width, unsigned int _height)	{ return true; }

		virtual void	commit(const MeshRenderItem& item)			{}
		virtual void	commit(DirectionalLight* light)				{}
		virtual void	commit(PointLight* light)						{}
		virtual void	commit(const DebugLine& line)					{}

		virtual void	render()										{}

		Camera&		getMainCamera()					{ return mainCamera; }

		bool			isWireMode() const				{ return wireMode; }
		void			setWireMode(bool enable)		{ wireMode = enable; }

		bool			isDebugMode() const				{ return debugMode; }
		void			setDebugMode(bool enable)		{ debugMode = enable; }

	protected:
		Camera	mainCamera;
		bool		wireMode;
		bool		debugMode;
	};
}