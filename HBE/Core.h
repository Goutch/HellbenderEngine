#pragma once

#if defined(_MSC_VER)
//  Microsoft
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//do nothing and hope for the best
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#if defined(HB_BUILD)
#define HB_API EXPORT
#define GLAD_GLAPI_EXPORT
#define GLAD_GLAPI_EXPORT_BUILD
#else
#define HB_API IMPORT
#define GLAD_GLAPI_EXPORT
#endif

#ifdef NDEBUG
#define RELEASE_MODE
#else
#define DEBUG_MODE
#endif

