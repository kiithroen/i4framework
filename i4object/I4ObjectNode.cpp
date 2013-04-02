#include "stdafx.h"
#include "I4ObjectNode.h"
#include "I4ObjectMgr.h"
#include "I4Camera.h"
#include "I4ObjectComponent.h"

namespace i4object {

	Object::Object(ObjectMgr* _objectMgr, const char* _name)
		: objectMgr(_objectMgr)
		, name(_name)
	{
		rotation.makeIdentity();
		position = VECTOR3_ZERO;
		scale = VECTOR3_ONE;

		worldTM.makeIdentity();
	}

	Object::~Object()
	{
		for (auto& itr : mapComponent)
		{
			itr.second->onRemove();
			delete itr.second;
		}
		mapComponent.clear();
	}

	void Object::updateTransformMatrix()
	{
		rotation.extractRotationMatrix(worldTM);
		worldTM.setPosition(position);
		if (scale != VECTOR3_ONE)
		{
			Matrix4x4 scaleTM;
			scaleTM.makeScale(scale.x, scale.y, scale.z);

			worldTM = scaleTM*worldTM;
		}
	}

	void Object::destroyFromScene()
	{
		objectMgr->destroyObject(this);
	}
	
	void Object::setLookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up)
	{
		position = eye;
		
		Matrix4x4 m;
		m.makeObjectLookAtLH(eye, at, up);

		rotation.makeRotationMatrix(m);

		updateTransformMatrix();
	}

	void Object::setRotationYawPitchRoll(float yaw, float pitch, float roll)
	{
		rotation.makeRotationYawPitchRoll(yaw, pitch, roll);

		updateTransformMatrix();
	}

	void Object::setPosition(const Vector3& _position)
	{
		position = _position;

		updateTransformMatrix();
	}

	void Object::setScale(const Vector3& _scale)
	{
		scale = _scale;

		updateTransformMatrix();
	}

	void Object::setRotation(const Quaternion& _rotation)
	{
		rotation = _rotation;

		updateTransformMatrix();
	}

	void Object::setWorldTM(const Matrix4x4& _worldTM)
	{
		worldTM = _worldTM;

		Matrix4x4 matRotation;
		worldTM.decompose(&scale, &matRotation, &position);
		rotation.makeRotationMatrix(matRotation);
	}

}