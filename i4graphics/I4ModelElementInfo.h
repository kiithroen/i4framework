#pragma once

#include "i4graphics.h"
#include "I4Matrix4x4.h"
using namespace i4core;

namespace i4graphics {

	struct I4ModelElementInfo
	{
		I4Matrix4x4	localTM;
		I4Matrix4x4	worldTM;
		int			parentID;
		string		parentName;
		string		name;
		string		type;
	};

}