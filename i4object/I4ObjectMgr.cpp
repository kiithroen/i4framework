#include "stdafx.h"
#include "I4ObjectMgr.h"
#include "I4ObjectNode.h"
#include "I4DeferredRenderer.h"
#include "I4ModelMgr.h"

namespace i4object {

	I4ObjectMgr::I4ObjectMgr()
		: renderer(nullptr)
		, modelMgr(nullptr)
	{
	}


	I4ObjectMgr::~I4ObjectMgr()
	{
		for (auto& itr : mapObject)
		{
			delete itr.second;
		}
		mapObject.clear();
	}

	bool I4ObjectMgr::init(I4Renderer* _renderer, I4ModelMgr* _modelMgr, I4PhysXMgr* _physXMgr)
	{
		renderer = _renderer;
		modelMgr = _modelMgr;
		physXMgr = _physXMgr;

		return true;
	}

	I4Object* I4ObjectMgr::createObject(const char* name)
	{
		assert(findObject(name) == nullptr);		// 같은 이름으로는 처음 생성되어야 한다.

		I4Object* sceneNode = new I4Object(this, name);
		mapObject.insert(make_pair(name, sceneNode));
		
		return sceneNode;
	}

	void I4ObjectMgr::destroyObject(I4Object* obj)
	{
		auto itr = mapObject.find(obj->getName());
		assert(itr != mapObject.end());

		delete itr->second;
		mapObject.erase(itr);
	}

	I4Object* I4ObjectMgr::findObject(const char* name)
	{
		auto itr = mapObject.find(name);
		if (itr == mapObject.end())
			return nullptr;

		return itr->second;
	}

}