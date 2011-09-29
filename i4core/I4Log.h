#pragma once

#include "i4core.h"

namespace i4core
{
	class I4CORE_API I4Log
	{
	public:
		enum Flag
		{
			FLAG_NONE		= 0,
			FLAG_CONSOLE	= 1 << 1,
			FLAG_DEBUGGER	= 1 << 2,
			FLAG_FILE		= 1 << 4,
			FLAG_XML		= 1 << 8,
		};

		enum Level
		{
			LEVEL_DEBUG		= 0,
			LEVEL_INFO		= 1,
			LEVEL_WARN		= 2,
			LEVEL_ERROR		= 3,
			LEVEL_FATAL		= 4,
		};

	public:
		I4Log(void);
		~I4Log(void);

		std::wostringstream& get(Level level, const char* file, const char* func, int line);

	private:
		const wchar_t* getLevelString(Level level) const;

	private:
		I4Log(const I4Log&);
		I4Log& operator = (const I4Log&);

	private:
		std::wostringstream	oss;

	public:
		static const int getReportFlag()			{ return reportFlag; }
		static const Level&	getReportLevel()		{ return reportLevel; }

		static void initialize(int flag, Level level, const wchar_t* fname = L"i4.log");
		static void finalize();

	private:
		static int				reportFlag;
		static Level			reportLevel;
		static std::ofstream	ofs;
	};

#define I4LOG(level, file, func, line) \
	if (I4Log::getReportFlag() == 0 || level < I4Log::getReportLevel()) ; \
	else I4Log().get(level, file, func, line)

#define I4LOG_DEBUG		I4LOG(I4Log::LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__)
#define I4LOG_INFO		I4LOG(I4Log::LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__)
#define I4LOG_WARN		I4LOG(I4Log::LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__)
#define I4LOG_ERROR		I4LOG(I4Log::LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__)
#define I4LOG_FATAL		I4LOG(I4Log::LEVEL_FATAL, __FILE__, __FUNCTION__, __LINE__)

}
