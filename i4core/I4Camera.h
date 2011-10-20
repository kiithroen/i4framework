#pragma once

#include "i4core.h"
#include "I4Sphere.h"
#include "I4AABB.h"
#include "I4Frustum.h"
#include "I4Quaternion.h"

namespace i4core
{
	class I4CORE_API I4Camera
	{
	public:
		I4Camera();
		~I4Camera();
		
		void				setPerspectiveFov(float fovY, float aspect, float zn, float zf);
		void				setLookAt(const I4Vector3& eye, const I4Vector3& at, const I4Vector3& up);

		bool				isVisiblePoint(const I4Vector3& p) const		{ return frustum.isInPoint(p); }
		bool				isVisibleSphere(const I4Sphere& sphere) const	{ return frustum.isInSphere(sphere); }
		bool				isVisibleAABB(const I4AABB& aabb) const			{ return frustum.isInAABB(aabb); }

		const I4Matrix4x4&	getProjectionMatrix() const						{ return projectionMatrix; }
		const I4Matrix4x4&	getViewMatrix()	const							{ return viewMatrix; }
		const I4Matrix4x4&	getWorldMatrix()	const						{ return worldMatrix; }
		const I4Matrix4x4&	getViewProjectionMatrix()	const				{ return viewProjectionMatrix; }
		
		const I4Vector3&	getPosition() const								{ return position; }
		const I4Vector3&	getRight() const								{ return right; }
		const I4Vector3&	getUp() const									{ return up; }
		const I4Vector3&	getDirection() const							{ return direction; }
		const I4Quaternion&	getRotation() const								{ return rotation; }

		float				getFovY() const									{ return fovY; }
		float				getAspect() const								{ return aspect; }
		float				getZNear() const								{ return zNear; }
		float				getZFar() const									{ return zFar; }

	private:
		void				updateVariable();

	private:
		I4Frustum			frustum;
		I4Matrix4x4			projectionMatrix;
		I4Matrix4x4			viewMatrix;	
		I4Matrix4x4			worldMatrix;
		I4Matrix4x4			viewProjectionMatrix;
		I4Quaternion		rotation;
		I4Vector3			position;
		I4Vector3			right;
		I4Vector3			direction;
		I4Vector3			up;
		float				fovY;
		float				aspect;
		float				zNear;
		float				zFar;
	};
}