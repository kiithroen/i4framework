#pragma once

#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	struct ModelElementInfo
	{
		Matrix4x4	localTM;
		Matrix4x4	worldTM;
		int			parentID;
		string		parentName;
		string		name;
		string		type;
		
		vector<int>	boneRefTable;
	};

}