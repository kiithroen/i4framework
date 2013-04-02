#pragma once

#include "I4Sphere.h"
#include "I4AABB.h"
#include "I4Frustum.h"
#include "I4Quaternion.h"

namespace i4core
{
	class Camera
	{
	public:
		Camera();
		~Camera();
		
		void				setPerspectiveFovLH(float fovY, float aspect, float zn, float zf);
		void				setOrthoOffCenterLH(float left, float right, float bottom, float top, float znearPlane, float zfarPlane);

		void				setLookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up);
		void				setViewMatrix(const Matrix4x4& view);

		bool				isVisiblePoint(const Vector3& p) const			{ return frustum.isInPoint(p); }
		bool				isVisibleSphere(const Vector3& p, float r) const	{ return frustum.isInSphere(p, r); }
		bool				isVisibleSphere(const Sphere& sphere) const		{ return frustum.isInSphere(sphere); }
		bool				isVisibleAABB(const AABB& aabb) const				{ return frustum.isInAABB(aabb); }

		const Matrix4x4&	getProjectionMatrix() const							{ return projectionMatrix; }
		const Matrix4x4&	getViewMatrix()	const								{ return viewMatrix; }
		const Matrix4x4&	getWorldMatrix()	const							{ return worldMatrix; }
		const Matrix4x4&	getViewProjectionMatrix()	const					{ return viewProjectionMatrix; }
		
		const Vector3&	getEye() const										{ return eye; }

		const Vector3&	getNearTopLeft() const								{ return nearTopLeft; }
		const Vector3&	getNearTopRight() const								{ return nearTopRight; }
		const Vector3&	getNearDownLeft() const								{ return nearDownLeft; }
		const Vector3&	getNearDownRight() const							{ return nearDownRight; }

		const Vector3&	getFarTopLeft() const								{ return farTopLeft; }
		const Vector3&	getFarTopRight() const								{ return farTopRight; }
		const Vector3&	getFarDownLeft() const								{ return farDownLeft; }
		const Vector3&	getFarDownRight() const								{ return farDownRight; }

		float				getFovY() const										{ return fovY; }
		float				getAspect() const									{ return aspect; }
		float				getZNear() const									{ return zNear; }
		float				getZFar() const										{ return zFar; }

		void				extractCorners(Vector3* corners) const;

	private:
		Frustum			frustum;

		Matrix4x4			projectionMatrix;
		Matrix4x4			viewMatrix;	
		Matrix4x4			worldMatrix;
		Matrix4x4			viewProjectionMatrix;

		Vector3			eye;

		Vector3			nearTopLeft;
		Vector3			nearTopRight;
		Vector3			nearDownLeft;
		Vector3			nearDownRight;

		Vector3			farTopLeft;
		Vector3			farTopRight;
		Vector3			farDownLeft;
		Vector3			farDownRight;

		float				fovY;
		float				aspect;
		float				zNear;
		float				zFar;
	};
}