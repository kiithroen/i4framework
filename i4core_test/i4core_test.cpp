// i4core_test.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "I4Log.h"
#include "I4StopWatch.h"
#include "I4Profile.h"
#include <iostream>

using namespace i4core;

void log_print(const char* fmt, ...)
{
	char msg[4096] = {0};

	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(msg, 4096, fmt, args);
	va_end(args);

	fprintf(stderr, "%s", msg);
	fflush(stderr);

	OutputDebugStringA(msg);
}

int _tmain(int argc, _TCHAR* argv[])
{
	I4Log::initialize(I4Log::FLAG_CONSOLE|I4Log::FLAG_DEBUGGER|I4Log::FLAG_FILE, I4Log::LEVEL_DEBUG, L"i4.log");
	I4StopWatch::initialize();

	I4StopWatch stopWatch;

	stopWatch.reset();
	for (int i = 0; i < 10; i++)
	{
		{
			PROFILE("debug");
			I4LOG_DEBUG << L"debug";
		}

		for (int j = 0; j < 10; j++)
		{
			PROFILE("info");
			I4LOG_INFO << L"info";
		}
			
		{
			PROFILE("warning");
			I4LOG_WARNING << L"warning";
		}

		for (int k = 0; k < 50; k++)
		{
			PROFILE("error");
			I4LOG_ERROR << L"error";
		}
		{
			PROFILE("fatal");
			I4LOG_FATAL << L"fatal";
		}
	}
	
	I4LOG_DEBUG << stopWatch.getElapsedTime();

	I4ProfileIterator* itr = I4ProfileManager::getIterator();

	do
	{
		I4LOG_DEBUG << itr->getCurChildName() << "\t" << itr->getCurChildTotalTime() << "\t" << itr->getCurChildTotalCalls();
		itr->next();
	} while (!itr->isDone());

	I4ProfileManager::releaseIterator(itr);

	I4Log::finalize();

	return 0;
}

