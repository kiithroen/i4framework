#pragma once

#include "i4core.h"

namespace Json
{
	class Value;
}

namespace i4core
{
	class I4ProfileNode;

	class I4CORE_API I4ProfileWriterJson
	{
	public:
		void write(I4ProfileNode* node, const wchar_t* fname);

	private:
		static void convertToJsonValue(I4ProfileNode* node, Json::Value& value);
	};
}