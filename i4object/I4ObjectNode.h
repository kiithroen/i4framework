#pragma once

#include "I4Matrix4x4.h"
#include "I4AABB.h"
#include "I4Quaternion.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object {

	class I4ObjectMgr;
	class I4ObjectComponent;

	typedef I4Messenger<I4ObjectComponent>			I4ObjectComponentMessenger;

	class I4ObjectNode
	{
		typedef vector<I4ObjectNode*>				I4ObjectNodeVector;
		typedef map<string, I4ObjectComponent*>		I4ObjectComponentMap;
	public:
		I4ObjectNode(I4ObjectMgr* objectMgr, I4ObjectNode* parent, const char* name);
		~I4ObjectNode();
				
		I4ObjectNode*			createChild(const char* name);

		void					attachChild(I4ObjectNode* child);

		void					detachChild(I4ObjectNode* child);
		void					detachFromParent();

		void					destroyFromScene();

		template <typename T>
		T* addComponent()
		{
			T* comp = new T;
			auto itr = mapComponent.find(comp->getComponentID());
			if (itr != mapComponent.end())	// 중복추가
			{
				delete comp;
				return nullptr;
			}

			comp->setOwner(this);
			comp->onAdd();

			mapComponent.insert(make_pair(comp->getComponentID(), comp));

			return comp;
		}

		template <typename T>
		T* findComponentAs(const string& familyID)
		{
			auto itr = mapComponent.find(familyID);
			if (itr == mapComponent.end())
				return NULL;

			return static_cast<T*>(itr->second);
		}

		void 					setLocalLookAt(const I4Vector3& eye, const I4Vector3& at, const I4Vector3& up);
		void 					setLocalTM(const I4Matrix4x4& localTM);
		void 					setLocalRotationYawPitchRoll(float yaw, float pitch, float roll);
		void 					setLocalPosition(const I4Vector3& t);
		void 					setLocalScale(const I4Vector3& s);

		I4ObjectMgr*					getObjectMgr()				{ return objectMgr; }
		I4ObjectComponentMessenger&		getMessenger()				{ return messenger; }

		const string&			getName()							{ return name; }
		
		const I4Matrix4x4&		getLocalTM() const					{ return localTM; }
		const I4Matrix4x4&		getWorldTM() const					{ return worldTM; }

	private:
		void					updateWorldTM();
		void					calcWorldTM();

		void					destroyAllChild();

	protected:
		I4Matrix4x4						localTM;
		I4Matrix4x4						worldTM;
		string							name;
		I4ObjectMgr*					objectMgr;
		I4ObjectNode*					parent;
		I4ObjectNodeVector				vecChild;
		I4ObjectComponentMap			mapComponent;
		I4ObjectComponentMessenger	messenger;
	};

}