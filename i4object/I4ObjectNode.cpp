#include "stdafx.h"
#include "I4ObjectNode.h"
#include "I4ObjectMgr.h"
#include "I4Camera.h"
#include "I4ObjectComponent.h"

namespace i4object {

	I4Object::I4Object(I4ObjectMgr* _objectMgr, const char* _name)
		: objectMgr(_objectMgr)
		, name(_name)
	{
		rotation.makeIdentity();
		position = I4VECTOR3_ZERO;
		scale = I4VECTOR3_ONE;

		worldTM.makeIdentity();
	}

	I4Object::~I4Object()
	{
		for (auto& itr : mapComponent)
		{
			itr.second->onRemove();
			delete itr.second;
		}
		mapComponent.clear();
	}

	void I4Object::updateTransformMatrix()
	{
		rotation.extractRotationMatrix(worldTM);
		worldTM.setPosition(position);
		if (scale != I4VECTOR3_ONE)
		{
			I4Matrix4x4 scaleTM;
			scaleTM.makeScale(scale.x, scale.y, scale.z);

			worldTM = scaleTM*worldTM;
		}
	}

	void I4Object::destroyFromScene()
	{
		objectMgr->destroyObject(this);
	}
	
	void I4Object::setLookAtLH(const I4Vector3& eye, const I4Vector3& at, const I4Vector3& up)
	{
		position = eye;
		
		I4Matrix4x4 m;
		m.makeObjectLookAtLH(eye, at, up);

		rotation.makeRotationMatrix(m);

		updateTransformMatrix();
	}

	void I4Object::setRotationYawPitchRoll(float yaw, float pitch, float roll)
	{
		rotation.makeRotationYawPitchRoll(yaw, pitch, roll);

		updateTransformMatrix();
	}

	void I4Object::setPosition(const I4Vector3& _position)
	{
		position = _position;

		updateTransformMatrix();
	}

	void I4Object::setScale(const I4Vector3& _scale)
	{
		scale = _scale;

		updateTransformMatrix();
	}

	void I4Object::setRotation(const I4Quaternion& _rotation)
	{
		rotation = _rotation;

		updateTransformMatrix();
	}

	void I4Object::setWorldTM(const I4Matrix4x4& _worldTM)
	{
		worldTM = _worldTM;

		I4Matrix4x4 matRotation;
		worldTM.decompose(&scale, &matRotation, &position);
		rotation.makeRotationMatrix(matRotation);
	}

}