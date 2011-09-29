#pragma once

#include "i4core.h"
#include "I4Profile.h"
#include <json/json.h>

namespace i4core
{
	class I4CORE_API I4ProfileWriterJson : public I4ProfileWriter
	{
	public:
		virtual void write(I4ProfileNode* node, const wchar_t* fname) override;

	private:
		static void convertToJsonValue(I4ProfileNode* node, Json::Value& value);
	};
}