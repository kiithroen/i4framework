#pragma once

#include "i4core.h"

namespace i4core
{
	class I4CORE_API I4StringUtil
	{
	public:
		static std::string to_char(const wchar_t* wstr);
		static std::wstring to_wchar_t(const char* str);
	};
}