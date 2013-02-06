#pragma once

#pragma warning(disable : 4100)
#pragma warning(disable : 4201)
#pragma warning(disable : 4251)
#pragma warning(disable : 4481)

#define _CRT_SECURE_NO_WARNINGS

// c++
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

// windows
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#include <windows.h>

// dll macro
#ifdef I4CORE_EXPORTS
#define I4CORE_API __declspec(dllexport)
#else
#define I4CORE_API __declspec(dllimport)
#endif
