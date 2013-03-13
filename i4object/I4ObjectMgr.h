#pragma once

#include "I4ObjectNode.h"
#include "I4Messenger.h"
using namespace i4core;

namespace i4graphics {
	class I4Renderer;
	class I4ModelMgr;
}

using namespace i4graphics;

namespace i4object {

	class I4PhysXMgr;
	class I4Object;

	class I4ObjectMgr
	{
		typedef map<string, I4Object*>			I4ObjectMap;
	public:
		I4ObjectMgr();
		~I4ObjectMgr();

		bool						init(I4Renderer* renderer, I4ModelMgr* modelMgr, I4PhysXMgr* physXMgr);

		I4Object*					createObject(const char* name);
		void						destroyObject(I4Object* obj);

		I4Object*					findObject(const char* name);

		I4ObjectComponentMessenger&	getMessenger()		{ return messenger; }
		I4Renderer*					getRenderer()		{ return renderer; }
		I4ModelMgr*					getModelMgr()		{ return modelMgr; }
		I4PhysXMgr*					getPhysXMgr()		{ return physXMgr; }

	private:
		I4ObjectMap					mapObject;
		I4ObjectComponentMessenger	messenger;
		I4Renderer*					renderer;
		I4ModelMgr*					modelMgr;
		I4PhysXMgr*					physXMgr;
	};

}
