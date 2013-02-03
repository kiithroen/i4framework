#pragma once

#include "i4core.h"

namespace i4core
{
	class I4CORE_API I4StringUtil
	{
	public:
		static const char* to_char(const wchar_t* wstr);
		static const wchar_t* to_wchar_t(const char* str);
	};
}