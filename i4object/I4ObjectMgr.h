#pragma once

#include "I4ObjectNode.h"

namespace i4graphics {
	class I4Renderer;
}
using namespace i4graphics;

namespace i4object {

	class I4ObjectNode;
	
	class I4OBJECT_API I4ObjectMgr
	{
		typedef map<string, I4ObjectNode*>					ObjectNodeMap;
	public:
		I4ObjectMgr();
		~I4ObjectMgr();
				
		I4ObjectNode*			createObjectNode(I4ObjectNode* parent, const char* name);
		void					destroyObjectNode(I4ObjectNode* node);

		I4ObjectNode*			findObjectNode(const char* name);

		I4ObjectNode*			getRootNode()		{ return rootNode; }

	private:
		I4ObjectNode*			rootNode;
		ObjectNodeMap			mapObjectNode;

	private:
		static I4ObjectMgr*		objectMgr;
	};

}
