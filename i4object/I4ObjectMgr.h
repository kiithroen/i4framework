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

	class I4ObjectNode;

	class I4OBJECT_API I4ObjectMgr
	{
		typedef map<string, I4ObjectNode*>			I4ObjectNodeMap;
	public:
		I4ObjectMgr();
		~I4ObjectMgr();
				
		I4ObjectNode*			createObjectNode(I4ObjectNode* parent, const char* name);
		void					destroyObjectNode(I4ObjectNode* node);

		I4ObjectNode*			findObjectNode(const char* name);

		I4ObjectNode*			getRootNode()			{ return rootNode; }

		I4ObjectComponentMessenger&	getMessenger()		{ return messenger; }
		I4Renderer*					getRenderer()		{ return renderer; }
		I4ModelMgr*					getModelMgr()		{ return modelMgr; }

	private:
		I4ObjectNode*					rootNode;
		I4ObjectNodeMap					mapObjectNode;
		I4ObjectComponentMessenger		messenger;
		I4Renderer*						renderer;
		I4ModelMgr*						modelMgr;
	};

}
