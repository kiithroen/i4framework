#include "I4StringUtil.h"

namespace i4core
{
	const char* I4StringUtil::to_char(const wchar_t* wstr)
	{
		int len = wcslen(wstr);

		char* pstr = (char*)malloc(sizeof(char)*len + 1);
		wcstombs(pstr, wstr, len+1);

		return pstr;
	}

	const wchar_t* I4StringUtil::to_wchar_t(const char* str)
	{
		int len = strlen(str)+1;

		wchar_t* pwstr = (wchar_t*)malloc(sizeof(wchar_t)*len);
		mbstowcs(pwstr, str, len);

		return pwstr;
	}

}