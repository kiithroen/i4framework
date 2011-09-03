#include "StdAfx.h"
#include "I4Log.h"

using namespace i4core;

int I4Log::reportFlag = I4Log::FLAG_CONSOLE|I4Log::FLAG_DEBUGGER|I4Log::FLAG_FILE;
I4Log::Level I4Log::reportLevel = I4Log::LEVEL_DEBUG;
std::wofstream I4Log::ofs;