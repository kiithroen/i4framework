#pragma once

namespace i4core
{
	class Log
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
		Log(void);
		~Log(void);

		wostringstream& get(Level level, const char* file, const char* func, int line);

	private:
		const wchar_t* getLevelString(Level level) const;

	private:
		Log(const Log&);
		Log& operator = (const Log&);

	private:
		wostringstream	oss;

	public:
		static const int getReportFlag()			{ return reportFlag; }
		static const Level&	getReportLevel()		{ return reportLevel; }

		static void initialize(int flag, Level level, const wchar_t* fname = L"i4.log");
		static void finalize();

	private:
		static int				reportFlag;
		static Level			reportLevel;
		static wofstream		ofs;
	};

#define LOG(level, file, func, line) \
	if (Log::getReportFlag() == 0 || level < Log::getReportLevel()) ; \
	else Log().get(level, file, func, line)

#define LOG_DEBUG		LOG(Log::LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__)
#define LOG_INFO		LOG(Log::LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__)
#define LOG_WARN		LOG(Log::LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__)
#define LOG_ERROR		LOG(Log::LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__)
#define LOG_FATAL		LOG(Log::LEVEL_FATAL, __FILE__, __FUNCTION__, __LINE__)

}
