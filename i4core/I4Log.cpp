#include "I4Log.h"

namespace i4core
{

	int I4Log::reportFlag = I4Log::FLAG_CONSOLE|I4Log::FLAG_DEBUGGER|I4Log::FLAG_FILE;
	I4Log::Level I4Log::reportLevel = I4Log::LEVEL_DEBUG;
	ofstream I4Log::ofs;

	I4Log::I4Log(void)
	{
	}

	I4Log::~I4Log(void)
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

	wostringstream& I4Log::get(Level level, const char* file, const char* func, int line)
	{
		oss << L"[" << getLevelString(level) << L"]";
		oss << L" ";

		return oss;
	}

	const wchar_t* I4Log::getLevelString(Level level) const
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

	void I4Log::initialize(int flag, Level level, const wchar_t* fname/* = L"i4.log"*/)
	{
		reportFlag = flag;
		reportLevel = level;

		if (reportFlag & FLAG_FILE)
		{
			ofs.open(fname);
		}

		I4LOG_INFO << "log initialized.";
	}

	void I4Log::finalize()
	{
		if (reportFlag & FLAG_FILE)
		{
			ofs.close();
		}
	}

}