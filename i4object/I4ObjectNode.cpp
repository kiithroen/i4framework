#include "stdafx.h"
#include "I4ObjectNode.h"
#include "I4ObjectMgr.h"
#include "I4Camera.h"
#include "I4ObjectComponent.h"

namespace i4object {

	I4ObjectNode::I4ObjectNode(I4ObjectMgr* _objectMgr, I4ObjectNode* _parent, const char* _name)
		: objectMgr(_objectMgr)
		, parent(_parent)
		, name(_name)
	{
		localRotation.makeIdentity();
		localScale.x = localScale.y = localScale.z = 1.0f;
		localPosition.x = localPosition.y = localPosition.z = 0.0f;

		localTM.makeIdentity();
		worldTM.makeIdentity();

		if (parent != nullptr)
		{
			parent->attachChild(this);
		}
	}

	I4ObjectNode::~I4ObjectNode()
	{
		for (auto& itr : mapComponent)
		{
			itr.second->onRemove();
			delete itr.second;
		}
		mapComponent.clear();
	}

	void I4ObjectNode::calcTM()
	{
		calcLocalTM();
		calcWorldTM();
		
		// 자식들 업데이트
		for (auto& itr : vecChild)
		{
			itr->calcTM();
		}
	}

	void I4ObjectNode::calcLocalTM()
	{		
		localRotation.extractRotationMatrix(localTM);

		if (localScale != I4VECTOR3_ONE)
		{
			I4Matrix4x4 scaleTM;
			scaleTM.makeScale(localScale.x, localScale.y, localScale.z);

			localTM *= scaleTM;
		}

		localTM._41 = localPosition.x;
		localTM._42 = localPosition.y;
		localTM._43 = localPosition.z;
		localTM._44 = 1.0f;
	}

	void I4ObjectNode::calcWorldTM()
	{
		if (parent == nullptr)			// 부모가 없으면 월드변환행렬 --> 로컬
		{
			worldTM = localTM;
		}
		else						// 부모가 있으면 월드변환행렬 --> 로컬 x 부모의 월드변환행렬
		{
			worldTM = localTM*parent->getWorldTM();
		}
	}

	I4ObjectNode* I4ObjectNode::createChild(const char* name)
	{
		return objectMgr->createObjectNode(this, name);
	}

	void I4ObjectNode::attachChild(I4ObjectNode* child)
	{
		child->detachFromParent();

		child->parent = this;
		child->calcTM();

		vecChild.push_back(child);
	}


	void I4ObjectNode::detachChild(I4ObjectNode* child)
	{
		for (auto itr = vecChild.begin(); itr != vecChild.end(); ++itr)
		{
			if (*itr == child)
			{
				(*itr)->parent = NULL;
				vecChild.erase(itr);
				return;
			}
		}
	}
	
	void I4ObjectNode::detachFromParent()
	{
		if (parent != nullptr)
		{
			parent->detachChild(this);
		}
	}

	void I4ObjectNode::destroyAllChild()
	{
		for (auto& itr : vecChild)
		{
			itr->destroyAllChild();
			objectMgr->destroyObjectNode(itr);
		}
		vecChild.clear();
	}

	void I4ObjectNode::destroyFromScene()
	{
		detachFromParent();		
		destroyAllChild();
		objectMgr->destroyObjectNode(this);
	}
	
	void I4ObjectNode::setLocalLookAt(const I4Vector3& eye, const I4Vector3& at, const I4Vector3& up)
	{
		I4Matrix4x4 m;
		m.makeObjectLookAtLH(eye, at, up);

		setLocalTM(m);
	}

	void I4ObjectNode::setLocalTM(const I4Matrix4x4& localTM)
	{
		I4Matrix4x4 rotation;
		localTM.decompose(&localScale, &rotation, &localPosition);
		localRotation.makeRotationMatrix(rotation);

		calcTM();
	}

	void I4ObjectNode::setLocalRotation(const I4Quaternion& rotation)
	{
		localRotation = rotation;

		calcTM();
	}

	void I4ObjectNode::setLocalRotationYawPitchRoll(float yaw, float pitch, float roll)
	{
		localRotation.makeRotationYawPitchRoll(yaw, pitch, roll);

		calcTM();
	}

	void I4ObjectNode::setLocalPosition(const I4Vector3& t)
	{
		localPosition = t;

		calcTM();
	}

	void I4ObjectNode::setLocalScale(const I4Vector3& s)
	{
		localScale = s;

		calcTM();
	}
}