#include "stdafx.h"
#include "I4Camera.h"

namespace i4core
{
	Camera::Camera()
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::setPerspectiveFovLH(float _fovY, float _aspect, float _zNear, float _zFar)
	{
		fovY = _fovY;
		aspect = _aspect;
		zNear = _zNear;
		zFar = _zFar;

		projectionMatrix.makePerspectiveFovLH(_fovY, _aspect, _zNear, _zFar);

		float hNear = 2*tan(fovY/2)*zNear;
		float wNear = hNear * aspect;

		Vector3 nearCenter = VECTOR3_AXISZ * zNear;

		nearTopLeft = nearCenter + (VECTOR3_AXISY * hNear/2) - (VECTOR3_AXISX * wNear/2);
		nearTopRight = nearCenter + (VECTOR3_AXISY * hNear/2) + (VECTOR3_AXISX * wNear/2);
		nearDownLeft = nearCenter - (VECTOR3_AXISY * hNear/2) - (VECTOR3_AXISX * wNear/2);
		nearDownRight = nearCenter - (VECTOR3_AXISY * hNear/2) + (VECTOR3_AXISX * wNear/2);

		float hFar = 2*tan(fovY/2)*zFar;
		float wFar = hFar * aspect;

		Vector3 farCenter = VECTOR3_AXISZ * zFar;

		farTopLeft = farCenter + (VECTOR3_AXISY * hFar/2) - (VECTOR3_AXISX * wFar/2);
		farTopRight = farCenter + (VECTOR3_AXISY * hFar/2) + (VECTOR3_AXISX * wFar/2);
		farDownLeft = farCenter - (VECTOR3_AXISY * hFar/2) - (VECTOR3_AXISX * wFar/2);
		farDownRight = farCenter - (VECTOR3_AXISY * hFar/2) + (VECTOR3_AXISX * wFar/2);

		viewProjectionMatrix = viewMatrix*projectionMatrix;

		frustum.make(viewProjectionMatrix);
	}
	
	void Camera::setOrthoOffCenterLH(float left, float right, float bottom, float top, float znearPlane, float zfarPlane)
	{
		zNear = znearPlane;
		zFar = zfarPlane;

		projectionMatrix.makeOrthoOffCenterLH(left, right, bottom, top, znearPlane, zfarPlane);
		
		nearTopLeft = Vector3(left, top, znearPlane); 
		nearTopRight = Vector3(right, top, znearPlane);
		nearDownLeft = Vector3(left, bottom, znearPlane);
		nearDownRight = Vector3(right, bottom, znearPlane);
		
		farTopLeft = Vector3(left, top, zfarPlane);
		farTopRight = Vector3(right, top, zfarPlane);
		farDownLeft = Vector3(left, bottom, zfarPlane);
		farDownRight = Vector3(right, bottom, zfarPlane);

		viewProjectionMatrix = viewMatrix*projectionMatrix;

		frustum.make(viewProjectionMatrix);
	}

	void Camera::setLookAtLH(const Vector3& _eye, const Vector3& _lookAt, const Vector3& _up)
	{
		viewMatrix.makeCameraLookAtLH(_eye, _lookAt, _up);

		setViewMatrix(viewMatrix);
	}

	void Camera::setViewMatrix(const Matrix4x4& view)
	{
		viewMatrix = view;
		viewMatrix.extractInversePrimitive(worldMatrix);

		eye = worldMatrix.getPosition();

		Matrix4x4::multiply(viewProjectionMatrix, viewMatrix, projectionMatrix);

		frustum.make(viewProjectionMatrix);
	}

	void Camera::extractCorners(Vector3* corners) const
	{
		corners[0] = nearTopLeft;
		corners[1] = nearTopRight;
		corners[2] = nearDownLeft;
		corners[3] = nearDownRight;
		corners[4] = farTopLeft;
		corners[5] = farTopRight;
		corners[6] = farDownLeft;
		corners[7] = farDownRight;
	}
}