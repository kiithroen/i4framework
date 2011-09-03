// i4core_test.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "I4Log.h"
using namespace i4core;

int _tmain(int argc, _TCHAR* argv[])
{	
	I4Log::initialize(I4Log::FLAG_CONSOLE|I4Log::FLAG_DEBUGGER|I4Log::FLAG_FILE, I4Log::LEVEL_DEBUG, L"i4.log");

	I4LOG_DEBUG << L"debug";
	I4LOG_INFO << L"info";
	I4LOG_WARNING << L"warning";
	I4LOG_ERROR << L"error";
	I4LOG_FATAL << L"fatal";

	I4Log::finalize();

	return 0;
}

