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