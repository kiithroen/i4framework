#pragma once

#include "i4core.h"

namespace i4core
{
	inline void to_string(string& out, const wstring& wstr)
	{
		char szTemp[256];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]), NULL, NULL );
		out = szTemp;
	}

	inline void to_wstring(wstring& out, const string& str)
	{
		wchar_t szTemp[256];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)strlen(str.c_str()) + 1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]));
		out = szTemp;
	}
}