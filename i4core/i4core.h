#pragma once

#pragma warning(disable : 4100)
#pragma warning(disable : 4201)
#pragma warning(disable : 4251)

// dll macro
#ifdef I4CORE_EXPORTS
#define I4CORE_API __declspec(dllexport)
#else
#define I4CORE_API __declspec(dllimport)
#endif