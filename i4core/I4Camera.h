#pragma once

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
		void				setOrthoOffCenter(float left, float right, float bottom, float top, float znearPlane, float zfarPlane);

		void				setLookAt(const I4Vector3& eye, const I4Vector3& lookAt, const I4Vector3& up);
		void				setTransform(const I4Quaternion& rotation, const I4Vector3& eye);
		void				setViewMatrix(const I4Matrix4x4& view);

		bool				isVisiblePoint(const I4Vector3& p) const			{ return frustum.isInPoint(p); }
		bool				isVisibleSphere(const I4Vector3& p, float r) const	{ return frustum.isInSphere(p, r); }
		bool				isVisibleSphere(const I4Sphere& sphere) const		{ return frustum.isInSphere(sphere); }
		bool				isVisibleAABB(const I4AABB& aabb) const				{ return frustum.isInAABB(aabb); }

		const I4Matrix4x4&	getProjectionMatrix() const							{ return projectionMatrix; }
		const I4Matrix4x4&	getViewMatrix()	const								{ return viewMatrix; }
		const I4Matrix4x4&	getWorldMatrix()	const							{ return worldMatrix; }
		const I4Matrix4x4&	getViewProjectionMatrix()	const					{ return viewProjectionMatrix; }

		const I4Quaternion&	getRotation() const									{ return rotation; }

		const I4Vector3&	getEye() const										{ return eye; }
		const I4Vector3&	getRight() const									{ return right; }
		const I4Vector3&	getUp() const										{ return up; }
		const I4Vector3&	getDirection() const								{ return direction; }

		const I4Vector3&	getNearTopLeft() const								{ return nearTopLeft; }
		const I4Vector3&	getNearTopRight() const								{ return nearTopRight; }
		const I4Vector3&	getNearDownLeft() const								{ return nearDownLeft; }
		const I4Vector3&	getNearDownRight() const							{ return nearDownRight; }

		const I4Vector3&	getFarTopLeft() const								{ return farTopLeft; }
		const I4Vector3&	getFarTopRight() const								{ return farTopRight; }
		const I4Vector3&	getFarDownLeft() const								{ return farDownLeft; }
		const I4Vector3&	getFarDownRight() const								{ return farDownRight; }

		float				getFovY() const										{ return fovY; }
		float				getAspect() const									{ return aspect; }
		float				getZNear() const									{ return zNear; }
		float				getZFar() const										{ return zFar; }

		void				extractCorners(I4Vector3* corners) const;

	private:
		I4Frustum			frustum;

		I4Matrix4x4			projectionMatrix;
		I4Matrix4x4			viewMatrix;	
		I4Matrix4x4			worldMatrix;
		I4Matrix4x4			viewProjectionMatrix;

		I4Quaternion		rotation;

		I4Vector3			eye;
		I4Vector3			right;
		I4Vector3			direction;
		I4Vector3			up;

		I4Vector3			nearTopLeft;
		I4Vector3			nearTopRight;
		I4Vector3			nearDownLeft;
		I4Vector3			nearDownRight;

		I4Vector3			farTopLeft;
		I4Vector3			farTopRight;
		I4Vector3			farDownLeft;
		I4Vector3			farDownRight;

		float				fovY;
		float				aspect;
		float				zNear;
		float				zFar;
	};
}