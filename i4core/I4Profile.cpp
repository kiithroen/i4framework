#include "I4Profile.h"

using namespace i4core;

I4ProfileNode	I4ProfileManager::root("root", NULL, NULL);
I4ProfileNode*	I4ProfileManager::curNode = &root;