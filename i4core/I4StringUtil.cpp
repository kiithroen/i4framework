#include "stdafx.h"
#include "I4StringUtil.h"

namespace i4core
{
	void StringUtil::toString(string& out, const wstring& wstr)
	{
		int len = (wstr.length() + 1)*2;

		char* pstr = (char*) malloc(sizeof(char)*len);
		wcstombs(pstr, wstr.c_str(), len);
		out = string(pstr);
		free(pstr);
	}

	void StringUtil::toWString(wstring& out, const string& str)
	{
		int len = str.length() + 1;
		wchar_t* pwstr = (wchar_t*)malloc(sizeof(wchar_t)*len);
		mbstowcs(pwstr, str.c_str(), len);
		out = wstring(pwstr);
		free(pwstr);
	}

	bool StringUtil::splitPath(wstring& dir, wstring& fname, wstring& ext, const wstring& path)
	{
		auto idx = path.find_last_of(L"\\/");
		if (std::wstring::npos != idx)
		{
			dir = path.substr(0, idx);
			fname = path.substr(idx + 1);

			idx = fname.find_last_of(L'.');
			if (std::wstring::npos != idx)
			{
				ext = fname.substr(idx + 1);
				fname = fname.substr(0, idx);
			}
		}

		return false;
	}

	wstring StringUtil::trimLeft(const wstring& delim, const wstring& source)
	{
		wstring::size_type n = source.find_first_not_of(delim);
		return n == wstring::npos ? L"" : source.substr(n);
	}

	wstring StringUtil::trimRight(const wstring& delim, const wstring& source)
	{
		wstring::size_type n = source.find_last_not_of(delim);
		return n == wstring::npos ? L"" : source.substr(0, n + 1);
	}

	wstring StringUtil::trim(const wstring& delim, const wstring& source)
	{
		return trimLeft(delim, trimRight(delim, source));
	}

	void StringUtil::split(vector<wstring>& result, const wstring& delim, const wstring& source, unsigned int maxsize)
	{
		wstring str = source;
		wstring::size_type start = 0;
		for (;;)
		{
			if (maxsize != 0 && result.size() == maxsize - 1)
			{
				result.push_back(str.substr(start));
				break;
			}

			wstring::size_type end = str.find_first_of(delim, start);
			if (end == wstring::npos)
			{
				result.push_back(str.substr(start));
				break;
			}

			result.push_back(str.substr(start, end - start));
			start = end + 1;
		}
	}

	void StringUtil::splitTrim(vector<wstring>& result, const wstring& delim, const wstring& source, unsigned int maxsize)
	{
		wstring str = trim(delim, source);

		wstring::size_type start = 0;
		wstring::size_type end = 0;

		for (;;)
		{
			if (maxsize != 0 && result.size() == maxsize - 1)
			{
				result.push_back(str.substr(start));
				break;
			}

			end = str.find_first_of(delim, start);
			if (end == wstring::npos)
			{
				result.push_back(str.substr(start));
				break;
			}

			result.push_back(str.substr(start, end - start));

			start = str.find_first_not_of(delim, end + 1);
		}
	}

	wstring StringUtil::toLower(const wstring& src)
	{
		wstring out;
		out.resize(src.length());
		transform(src.begin(), src.end(), out.begin(), tolower);
		return out;
	}

	wstring StringUtil::toUpper(const wstring& src)
	{
		wstring out;
		out.resize(src.length());
		transform(src.begin(), src.end(), out.begin(), toupper);
		return out;
	}
}