#include "stdafx.h"
#include "I4ObjectNode.h"
#include "I4ObjectMgr.h"
#include "I4Camera.h"

namespace i4object {

	I4ObjectNode::I4ObjectNode(I4ObjectNode* _parent, const char* _name)
		: parent(_parent)
		, name(_name)
	{
		localRotation.makeIdentity();
		localScale.x = localScale.y = localScale.z = 1.0f;
		localPosition.x = localPosition.y = localPosition.z = 0.0f;

		localTM.makeIdentity();
		worldTM.makeIdentity();

		if (parent != NULL)
		{
			parent->addChild(this);
		}
	}

	I4ObjectNode::~I4ObjectNode()
	{
	}

	void I4ObjectNode::calcTM()
	{
		calcLocalTM();
		calcWorldTM();
		
		// 자식들 업데이트
		for (ObjectNodeVector::iterator itr = vecChild.begin(); itr != vecChild.end(); ++itr)
		{
			(*itr)->calcTM();
		}
	}

	void I4ObjectNode::calcLocalTM()
	{		
		localRotation.extractRotationMatrix(localTM);

		localTM._41 = localPosition.x;
		localTM._42 = localPosition.y;
		localTM._43 = localPosition.z;
		localTM._44 = 1.0f;

		if (localScale != I4Vector3(1, 1, 1))
		{
			I4Matrix4x4 scaleTM;
			scaleTM.makeScale(localScale.x, localScale.y, localScale.z);

			localTM *= scaleTM;
		}
	}

	void I4ObjectNode::calcWorldTM()
	{
		if (parent == NULL)			// 부모가 없으면 월드변환행렬 --> 로컬
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
		return I4ObjectMgr::getObjectMgr()->createObjectNode(this, name);
	}

	void I4ObjectNode::addChild(I4ObjectNode* child)
	{
		child->parent = this;
		child->calcTM();

		vecChild.push_back(child);
	}


	void I4ObjectNode::removeChild(I4ObjectNode* child)
	{
		for (ObjectNodeVector::iterator itr = vecChild.begin(); itr != vecChild.end(); ++itr)
		{
			if (*itr == child)
			{
				(*itr)->parent = NULL;

				vecChild.erase(itr);
				return;
			}
		}
	}
	
	void I4ObjectNode::removeFromParent()
	{
		if (parent != NULL)
		{
			parent->removeChild(this);
		}
	}

	void I4ObjectNode::destroyChild(I4ObjectNode* child)
	{
		removeChild(child);
		I4ObjectMgr::getObjectMgr()->destroyObjectNode(child);
	}

	void I4ObjectNode::destroyChildAll()
	{
		for (ObjectNodeVector::iterator itr = vecChild.begin(); itr != vecChild.end(); ++itr)
		{
			(*itr)->destroyChildAll();
			I4ObjectMgr::getObjectMgr()->destroyObjectNode(*itr);
		}
		vecChild.clear();
	}

	void I4ObjectNode::destroyFromObject()
	{
		removeFromParent();
		destroyChildAll();
		I4ObjectMgr::getObjectMgr()->destroyObjectNode(this);
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