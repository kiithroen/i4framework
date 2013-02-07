#pragma once


#define _CRTDBG_MAP_ALLOC

// i4core
#include "i4core.h"

// dll macro
#ifdef I4GRAPHICS_EXPORTS
#define I4GRAPHICS_API __declspec(dllexport)
#else
#define I4GRAPHICS_API __declspec(dllimport)
#endif
