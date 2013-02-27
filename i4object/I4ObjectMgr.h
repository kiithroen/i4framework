#pragma once

#include "I4ObjectNode.h"

namespace i4object {

	class I4ObjectNode;
	
	class I4OBJECT_API I4ObjectMgr
	{
		typedef map<string, I4ObjectNode*>					ObjectNodeMap;
	public:
		I4ObjectMgr();
		~I4ObjectMgr();
		
		void 			update(float dt);
		void 			render();
		
		I4ObjectNode*		createObjectNode(I4ObjectNode* parent, const char* name);
		I4ObjectNode*		findObjectNode(const char* name);
		void				destroyObjectNode(I4ObjectNode* node);

		I4ObjectNode*		getRootNode()		{ return rootNode; }
		
	public:
		static I4ObjectMgr*	getObjectMgr()		{ return objectMgr; }
		static void			createObjectMgr();
		static void			destroyObjectMgr();

	private:
		I4ObjectNode*					rootNode;
		ObjectNodeMap					mapObjectNode;

	private:
		static I4ObjectMgr*		objectMgr;
	};

}
