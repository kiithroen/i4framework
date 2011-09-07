// i4core_test.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "I4Log.h"
#include "I4StopWatch.h"
#include "I4Profile.h"

using namespace i4core;

int _tmain(int argc, _TCHAR* argv[])
{
	I4Log::initialize(I4Log::FLAG_CONSOLE|I4Log::FLAG_DEBUGGER|I4Log::FLAG_FILE, I4Log::LEVEL_DEBUG, L"i4.log");
	I4StopWatch::initialize();

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

