#include "stdafx.h"
#include "I4ObjectMgr.h"
#include "I4ObjectNode.h"
#include "I4DeferredRenderer.h"
#include "I4ModelMgr.h"

namespace i4object {

	I4ObjectMgr::I4ObjectMgr()
		: rootNode(nullptr)
		, renderer(nullptr)
		, modelMgr(nullptr)
	{
		rootNode = createObjectNode(nullptr, "root");

	}


	I4ObjectMgr::~I4ObjectMgr()
	{
		for (auto& itr : mapObjectNode)
		{
			delete itr.second;
		}
		mapObjectNode.clear();
		
		rootNode = nullptr;
	}

	bool I4ObjectMgr::init(I4Renderer* _renderer, I4ModelMgr* _modelMgr)
	{
		renderer = _renderer;
		modelMgr = _modelMgr;

		return true;
	}

	I4ObjectNode* I4ObjectMgr::createNode(const char* name)
	{
		return rootNode->createChild(name);
	}

	I4ObjectNode* I4ObjectMgr::createObjectNode(I4ObjectNode* parent, const char* name)
	{
		assert(findObjectNode(name) == nullptr);		// 같은 이름으로는 처음 생성되어야 한다.

		I4ObjectNode* sceneNode = new I4ObjectNode(this, parent, name);
		mapObjectNode.insert(make_pair(name, sceneNode));
		
		return sceneNode;
	}

	void I4ObjectMgr::destroyObjectNode(I4ObjectNode* node)
	{
		auto itr = mapObjectNode.find(node->getName());
		assert(itr != mapObjectNode.end());

		delete itr->second;
		mapObjectNode.erase(itr);
	}

	I4ObjectNode* I4ObjectMgr::findObjectNode(const char* name)
	{
		auto itr = mapObjectNode.find(name);
		if (itr == mapObjectNode.end())
			return nullptr;

		return itr->second;
	}

}