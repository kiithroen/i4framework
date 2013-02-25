#include "stdafx.h"
#include "I4Camera.h"

namespace i4core
{
	I4Camera::I4Camera()
	{
	}

	I4Camera::~I4Camera()
	{
	}

	void I4Camera::setPerspectiveFov(float _fovY, float _aspect, float _zNear, float _zFar)
	{
		fovY = _fovY;
		aspect = _aspect;
		zNear = _zNear;
		zFar = _zFar;

		projectionMatrix.makePerspectiveFovLH(_fovY, _aspect, _zNear, _zFar);

		float hNear = 2*tan(fovY/2)*zNear;
		float wNear = hNear * aspect;

		I4Vector3 nearCenter = I4VECTOR3_AXISZ * zNear;

		nearTopLeft = nearCenter + (I4VECTOR3_AXISY * hNear/2) - (I4VECTOR3_AXISX * wNear/2);
		nearTopRight = nearCenter + (I4VECTOR3_AXISY * hNear/2) + (I4VECTOR3_AXISX * wNear/2);
		nearDownLeft = nearCenter - (I4VECTOR3_AXISY * hNear/2) - (I4VECTOR3_AXISX * wNear/2);
		nearDownRight = nearCenter - (I4VECTOR3_AXISY * hNear/2) + (I4VECTOR3_AXISX * wNear/2);

		float hFar = 2*tan(fovY/2)*zFar;
		float wFar = hFar * aspect;

		I4Vector3 farCenter = I4VECTOR3_AXISZ * zFar;

		farTopLeft = farCenter + (I4VECTOR3_AXISY * hFar/2) - (I4VECTOR3_AXISX * wFar/2);
		farTopRight = farCenter + (I4VECTOR3_AXISY * hFar/2) + (I4VECTOR3_AXISX * wFar/2);
		farDownLeft = farCenter - (I4VECTOR3_AXISY * hFar/2) - (I4VECTOR3_AXISX * wFar/2);
		farDownRight = farCenter - (I4VECTOR3_AXISY * hFar/2) + (I4VECTOR3_AXISX * wFar/2);

		I4Matrix4x4::multiply(viewProjectionMatrix, viewMatrix, projectionMatrix);

		frustum.make(viewProjectionMatrix);
	}
	
	void I4Camera::setOrthoOffCenter(float left, float right, float bottom, float top, float znearPlane, float zfarPlane)
	{
		zNear = znearPlane;
		zFar = zfarPlane;

		projectionMatrix.makeOrthoOffCenterLH(left, right, bottom, top, znearPlane, zfarPlane);
		
		nearTopLeft = I4Vector3(left, top, znearPlane); 
		nearTopRight = I4Vector3(right, top, znearPlane);
		nearDownLeft = I4Vector3(left, bottom, znearPlane);
		nearDownRight = I4Vector3(right, bottom, znearPlane);
		
		farTopLeft = I4Vector3(left, top, zfarPlane);
		farTopRight = I4Vector3(right, top, zfarPlane);
		farDownLeft = I4Vector3(left, bottom, zfarPlane);
		farDownRight = I4Vector3(right, bottom, zfarPlane);

		I4Matrix4x4::multiply(viewProjectionMatrix, viewMatrix, projectionMatrix);

		frustum.make(viewProjectionMatrix);
	}

	void I4Camera::setLookAt(const I4Vector3& _eye, const I4Vector3& _lookAt, const I4Vector3& _up)
	{
		eye = _eye;
		up = _up;

		viewMatrix.makeCameraLookAtLH(_eye, _lookAt, _up);
		viewMatrix.extractInversePrimitive(worldMatrix);

		worldMatrix.extractAxisX(right);
		worldMatrix.extractAxisZ(direction);

		I4Matrix4x4 rotationMatrix;
		worldMatrix.decompose(nullptr, &rotationMatrix, nullptr);
		rotation.makeRotationMatrix(rotationMatrix);

		I4Matrix4x4::multiply(viewProjectionMatrix, viewMatrix, projectionMatrix);

		frustum.make(viewProjectionMatrix);
	}

	void I4Camera::setTransform(const I4Quaternion& _rotation, const I4Vector3& _eye)
	{
		rotation = _rotation;
		eye = _eye;

		rotation.extractRotationMatrix(worldMatrix);
		worldMatrix.setTranslation(_eye);
		worldMatrix.extractInversePrimitive(viewMatrix);

		worldMatrix.extractAxisX(right);
		worldMatrix.extractAxisY(up);
		worldMatrix.extractAxisZ(direction);

		I4Matrix4x4::multiply(viewProjectionMatrix, viewMatrix, projectionMatrix);

		frustum.make(viewProjectionMatrix);
	}

	void I4Camera::setViewMatrix(const I4Matrix4x4& view)
	{
		viewMatrix = view;
		viewMatrix.extractInversePrimitive(worldMatrix);

		worldMatrix.extractAxisX(right);
		worldMatrix.extractAxisY(up);
		worldMatrix.extractAxisZ(direction);
		worldMatrix.extractTranslation(eye);

		I4Matrix4x4 rotationMatrix;
		worldMatrix.decompose(nullptr, &rotationMatrix, nullptr);
		rotation.makeRotationMatrix(rotationMatrix);

		I4Matrix4x4::multiply(viewProjectionMatrix, viewMatrix, projectionMatrix);

		frustum.make(viewProjectionMatrix);
	}

	void I4Camera::extractCorners(I4Vector3* corners) const
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