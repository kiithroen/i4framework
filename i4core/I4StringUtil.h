#pragma once

#include "i4core.h"

namespace i4core
{
	class StringUtil
	{
	public:
		static void		toString(string& out, const wstring& wstr);
		static void		toWString(wstring& out, const string& str);

		static bool		splitPath(wstring& dir, wstring& fname, wstring& ext, const wstring& path);

		static wstring	trimLeft(const wstring& delim, const wstring& source);
		static wstring	trimRight(const wstring& delim, const wstring& source);
		static wstring	trim(const wstring& delim, const wstring& source);

		static void		split(vector<wstring>& result, const wstring& delim, const wstring& source, unsigned int maxsize = 0);
		static void		split(vector<wstring>& result, const char delim, const wstring& source, unsigned int maxsize = 0);
		static void		splitTrim(vector<wstring>& result, const wstring& delim, const wstring& source, unsigned int maxsize = 0);

		static wstring	toLower(const wstring& src);
		static wstring	toUpper(const wstring& src);
	};
}