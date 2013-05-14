#pragma once

#include "I4Matrix4x4.h"
#include "I4AABB.h"
#include "I4Quaternion.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4object {

	class ObjectMgr;
	class ObjectComponent;

	typedef Messenger<ObjectComponent>			ObjectComponentMessenger;

	class Object
	{
		typedef map<string, ObjectComponent*>		ObjectComponentMap;
	public:
		Object(ObjectMgr* objectMgr, const char* name);
		~Object();

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

		void 					setLookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up);
		void 					setRotationYawPitchRoll(float yaw, float pitch, float roll);
		void 					setPosition(const Vector3& position);
		void 					setScale(const Vector3& scale);
		void					setRotation(const Quaternion& rotation);
		void					setWorldTM(const Matrix4x4& worldTM);

		ObjectMgr*					getObjectMgr()	{ return objectMgr; }
		ObjectComponentMessenger&	getMessenger()	{ return messenger; }

		const string&			getName()				{ return name; }

		const Quaternion&		getRotation() const		{ return rotation; }
		const Vector3&			getPosition() const		{ return position; }
		const Vector3&			getScale() const		{ return scale; }

		const Matrix4x4&		getWorldTM() const		{ return worldTM; }

	private:
		void					updateTransformMatrix();

	protected:
		Quaternion					rotation;
		Vector3						scale;
		Vector3						position;
		Matrix4x4					worldTM;
		string						name;

		ObjectMgr*					objectMgr;
		ObjectComponentMap			mapComponent;
		ObjectComponentMessenger	messenger;
	};

}