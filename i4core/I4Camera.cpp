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

		projectionMatrix.makePerspectiveFovLH(fovY, aspect, zNear, zFar);

		I4Vector3 axisX = I4Vector3(1, 0, 0);
		I4Vector3 axisY = I4Vector3(0, 1, 0);
		I4Vector3 axisZ = I4Vector3(0, 0, 1);

		float hNear = 2*tan(fovY/2)*zNear;
		float wNear = hNear * aspect;

		I4Vector3 nearCenter = axisZ * zNear;

		nearTopLeft = nearCenter + (axisY * hNear/2) - (axisX * wNear/2);
		nearTopRight = nearCenter + (axisY * hNear/2) + (axisX * wNear/2);
		nearDownLeft = nearCenter - (axisY * hNear/2) - (axisX * wNear/2);
		nearDownRight = nearCenter - (axisY * hNear/2) + (axisX * wNear/2);

		float hFar = 2*tan(fovY/2)*zFar;
		float wFar = hFar * aspect;

		I4Vector3 farCenter = axisZ * zFar;

		farTopLeft = farCenter + (axisY * hFar/2) - (axisX * wFar/2);
		farTopRight = farCenter + (axisY * hFar/2) + (axisX * wFar/2);
		farDownLeft = farCenter - (axisY * hFar/2) - (axisX * wFar/2);
		farDownRight = farCenter - (axisY * hFar/2) + (axisX * wFar/2);

		updateVariable();
	}
	
	void I4Camera::setLookAt(const I4Vector3& eye, const I4Vector3& at, const I4Vector3& up)
	{		
		viewMatrix.makeCameraLookAtLH(eye, at, up);

		updateVariable();
	}

	void I4Camera::updateVariable()
	{
		I4Matrix4x4::multiply(viewProjectionMatrix, viewMatrix, projectionMatrix);

		viewMatrix.extractInversePrimitive(worldMatrix);

		worldMatrix.extractTranslation(position);
		worldMatrix.extractAxisX(right);
		worldMatrix.extractAxisY(up);
		worldMatrix.extractAxisZ(direction);

		I4Matrix4x4 rotationMatrix;
		worldMatrix.decompose(NULL, &rotationMatrix, NULL);
		rotation.makeRotationMatrix(rotationMatrix);

		frustum.make(getViewProjectionMatrix());
	}

}