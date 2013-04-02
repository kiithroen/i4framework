#include "stdafx.h"
#include "I4Log.h"

namespace i4core
{

	int Log::reportFlag = Log::FLAG_CONSOLE|Log::FLAG_DEBUGGER|Log::FLAG_FILE;
	Log::Level Log::reportLevel = Log::LEVEL_DEBUG;
	wofstream Log::ofs;

	Log::Log(void)
	{
	}

	Log::~Log(void)
	{
		oss << endl;

		if (reportFlag & FLAG_CONSOLE)
		{
			fwprintf(stderr, L"%s", oss.str().c_str());
			fflush(stderr);
		}

		if (reportFlag & FLAG_DEBUGGER)
		{
			OutputDebugString(oss.str().c_str());
		}

		if (reportFlag & FLAG_FILE)
		{
			ofs << oss.str().c_str();
		}
	}

	wostringstream& Log::get(Level level, const char* file, const char* func, int line)
	{
		oss << L"[" << getLevelString(level) << L"]";
		oss << L" ";

		return oss;
	}

	const wchar_t* Log::getLevelString(Level level) const
	{
		static const wchar_t* LEVEL_STRING[] =
		{
			L"DEBUG",
			L"INFO",
			L"WARNING",
			L"ERROR",
			L"FATAL",
		};

		return LEVEL_STRING[level];
	}

	void Log::initialize(int flag, Level level, const wchar_t* fname/* = L"i4.log"*/)
	{
		reportFlag = flag;
		reportLevel = level;

		if (reportFlag & FLAG_FILE)
		{
			ofs.open(fname);
		}

		LOG_INFO << "log initialized.";
	}

	void Log::finalize()
	{
		if (reportFlag & FLAG_FILE)
		{
			ofs.close();
		}
	}

}