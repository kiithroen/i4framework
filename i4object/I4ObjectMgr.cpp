#include "stdafx.h"
#include "I4ObjectMgr.h"
#include "I4ObjectNode.h"

namespace i4object {

	I4ObjectMgr::I4ObjectMgr()
		: rootNode(NULL)
	{
		rootNode = createObjectNode(NULL, "root");
	}


	I4ObjectMgr::~I4ObjectMgr()
	{
		for (ObjectNodeMap::iterator itr = mapObjectNode.begin(); itr != mapObjectNode.end(); ++itr)
		{
			delete itr->second;
		}
		mapObjectNode.clear();

		rootNode = NULL;
	}
	
	void I4ObjectMgr::update(float dt)
	{
	}

	void I4ObjectMgr::render()
	{
	}

	I4ObjectNode* I4ObjectMgr::createObjectNode(I4ObjectNode* parent, const char* name)
	{
		assert(findObjectNode(name) == NULL);		// 같은 이름으로는 처음 생성되어야 한다.

		I4ObjectNode* sceneNode = new I4ObjectNode(parent, name);
		mapObjectNode.insert(make_pair(name, sceneNode));
		
		return sceneNode;
	}

	I4ObjectNode* I4ObjectMgr::findObjectNode(const char* name)
	{
		ObjectNodeMap::iterator itr = mapObjectNode.find(name);
		if (itr == mapObjectNode.end())
			return NULL;

		return itr->second;
	}

	void I4ObjectMgr::destroyObjectNode(I4ObjectNode* node)
	{
		ObjectNodeMap::iterator itr = mapObjectNode.find(node->getName());
		assert(itr != mapObjectNode.end());

		delete itr->second;
		mapObjectNode.erase(itr);
	}
	
	//---------------------------------------------------------------

	I4ObjectMgr* I4ObjectMgr::objectMgr = NULL;

	void I4ObjectMgr::createObjectMgr()
	{
		objectMgr = new I4ObjectMgr;
	}

	void I4ObjectMgr::destroyObjectMgr()
	{
		delete objectMgr;
		objectMgr = NULL;
	}
}