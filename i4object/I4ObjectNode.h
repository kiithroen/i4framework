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

	class I4Object
	{
		typedef map<string, I4ObjectComponent*>		I4ObjectComponentMap;
	public:
		I4Object(I4ObjectMgr* objectMgr, const char* name);
		~I4Object();

		void destroyFromScene();

		template <typename T>
		T* addComponent()
		{
			T* comp = new T;
			auto itr = mapComponent.find(T::getComponentID());
			if (itr != mapComponent.end())	// 중복추가
			{
				delete comp;
				return nullptr;
			}

			comp->setOwner(this);
			comp->onAdd();

			mapComponent.insert(make_pair(T::getComponentID(), comp));

			return comp;
		}

		template <typename T>
		T* findComponent()
		{
			auto itr = mapComponent.find(T::getComponentID());
			if (itr == mapComponent.end())
				return NULL;

			return static_cast<T*>(itr->second);
		}

		void 					setLookAtLH(const I4Vector3& eye, const I4Vector3& at, const I4Vector3& up);
		void 					setRotationYawPitchRoll(float yaw, float pitch, float roll);
		void 					setPosition(const I4Vector3& position);
		void 					setScale(const I4Vector3& scale);
		void					setRotation(const I4Quaternion& rotation);
		void					setWorldTM(const I4Matrix4x4& worldTM);

		I4ObjectMgr*					getObjectMgr()	{ return objectMgr; }
		I4ObjectComponentMessenger&		getMessenger()	{ return messenger; }

		const string&			getName()				{ return name; }

		const I4Quaternion&		getRotation() const		{ return rotation; }
		const I4Vector3&		getPosition() const		{ return position; }
		const I4Vector3&		getScale() const		{ return scale; }

		const I4Matrix4x4&		getWorldTM() const		{ return worldTM; }

	private:
		void					updateTransformMatrix();

	protected:
		I4Quaternion					rotation;
		I4Vector3						scale;
		I4Vector3						position;
		I4Matrix4x4						worldTM;
		string							name;

		I4ObjectMgr*					objectMgr;
		I4ObjectComponentMap			mapComponent;
		I4ObjectComponentMessenger		messenger;
	};

}