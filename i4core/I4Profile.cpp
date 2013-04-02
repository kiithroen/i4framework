#include "stdafx.h"
#include "I4Profile.h"

namespace i4core
{

	ProfileNode	ProfileManager::rootNode("root", nullptr, nullptr);
	ProfileNode*	ProfileManager::curNode = &rootNode;

	ProfileNode::ProfileNode(const char* _name, ProfileNode* _parent, ProfileNode* _sibling)
	: totalTime(0)
	, totalCalls(0)
	, recursionCounter(0)
	, parent(_parent)
	, child(nullptr)		
	, sibling(_sibling)
	, name(_name)
	{
	};

	ProfileNode::~ProfileNode()
	{
		delete child;
		delete sibling;
	}

}