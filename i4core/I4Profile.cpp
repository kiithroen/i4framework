#include "I4Profile.h"

namespace i4core
{

	I4ProfileNode	I4ProfileManager::rootNode("root", NULL, NULL);
	I4ProfileNode*	I4ProfileManager::curNode = &rootNode;

	I4ProfileNode::I4ProfileNode(const char* _name, I4ProfileNode* _parent, I4ProfileNode* _sibling)
	: totalTime(0)
	, totalCalls(0)
	, recursionCounter(0)
	, parent(_parent)
	, child(NULL)		
	, sibling(_sibling)
	, name(_name)
	{
	};

	I4ProfileNode::~I4ProfileNode()
	{
		delete child;
		delete sibling;
	}

}