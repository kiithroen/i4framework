#pragma once

// i4core
#include "i4core.h"
#include "i4graphics.h"

// dll macro
#ifdef I4OBJECT_EXPORTS
#define I4OBJECT_API __declspec(dllexport)
#else
#define I4OBJECT_API __declspec(dllimport)
#endif
