#pragma once

#pragma warning(disable : 4100)
#pragma warning(disable : 4201)
#pragma warning(disable : 4251)
#pragma warning(disable : 4481)

// dll macro
#ifdef I4CORE_EXPORTS
#define I4CORE_API __declspec(dllexport)
#else
#define I4CORE_API __declspec(dllimport)
#endif

// c++
#include <sstream>
#include <fstream>

// windows
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>