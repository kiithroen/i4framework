#include "stdafx.h"
#include "I4ObjectMgr.h"
#include "I4ObjectNode.h"
#include "I4DeferredRenderer.h"
#include "I4ModelMgr.h"

namespace i4object {

	ObjectMgr::ObjectMgr()
		: renderer(nullptr)
		, modelMgr(nullptr)
	{
	}


	ObjectMgr::~ObjectMgr()
	{
		for (auto& itr : mapObject)
		{
			delete itr.second;
		}
		mapObject.clear();
	}

	bool ObjectMgr::init(Renderer* _renderer, ModelMgr* _modelMgr, PhysXMgr* _physXMgr)
	{
		renderer = _renderer;
		modelMgr = _modelMgr;
		physXMgr = _physXMgr;

		return true;
	}

	Object* ObjectMgr::createObject(const char* name)
	{
		assert(findObject(name) == nullptr);		// 같은 이름으로는 처음 생성되어야 한다.

		auto sceneNode = new Object(this, name);
		mapObject.insert(make_pair(name, sceneNode));
		
		return sceneNode;
	}

	void ObjectMgr::destroyObject(Object* obj)
	{
		auto itr = mapObject.find(obj->getName());
		assert(itr != mapObject.end());

		delete itr->second;
		mapObject.erase(itr);
	}

	Object* ObjectMgr::findObject(const char* name)
	{
		auto itr = mapObject.find(name);
		if (itr == mapObject.end())
			return nullptr;

		return itr->second;
	}

}