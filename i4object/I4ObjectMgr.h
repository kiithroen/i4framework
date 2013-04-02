#pragma once

#include "I4ObjectNode.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4graphics {
	class Renderer;
	class ModelMgr;
}

using namespace i4graphics;

namespace i4object {

	class PhysXMgr;
	class Object;

	class ObjectMgr
	{
		typedef map<string, Object*>			ObjectMap;
	public:
		ObjectMgr();
		~ObjectMgr();

		bool						init(Renderer* renderer, ModelMgr* modelMgr, PhysXMgr* physXMgr);

		Object*					createObject(const char* name);
		void						destroyObject(Object* obj);

		Object*					findObject(const char* name);

		ObjectComponentMessenger&	getMessenger()		{ return messenger; }
		Renderer*					getRenderer()		{ return renderer; }
		ModelMgr*					getModelMgr()		{ return modelMgr; }
		PhysXMgr*					getPhysXMgr()		{ return physXMgr; }

	private:
		ObjectMap					mapObject;
		ObjectComponentMessenger	messenger;
		Renderer*					renderer;
		ModelMgr*					modelMgr;
		PhysXMgr*					physXMgr;
	};

}
