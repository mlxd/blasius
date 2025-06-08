/**
 * @brief System macros for dynamic library loading and function resolution.
 * @author Lee J. O'Riordan (loriordan@gmail.com)
 * @version 0.1
 * @date 2025-06-08
 * @copyright Copyright (c) 2025
 */
#pragma once

// Adapted from
// https://stackoverflow.com/questions/53530566/loading-dll-in-windows-c-for-cross-platform-design
#if defined(__unix__) || defined(__linux__)
#include <dlfcn.h>
#define DYNHANDLE              void*
#define DYNOPEN(lib_path, ...) dlopen(lib_path __VA_OPT__(, ) __VA_ARGS__)
#define DYNSYM(lib, fn)        dlsym(lib, (fn))
#define DYNCLOSE(lib)          dlclose(lib)
#define DYNERROR               dlerror

#elif defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#define DYNHANDLE              HINSTANCE
#define DYNOPEN(lib_path, ...) LoadLibraryEx(lib_path __VA_OPT__(, ) __VA_ARGS__)
#define DYNSYM(lib, fn)        GetProcAddress(lib, (fn))
#define DYNCLOSE(lib)          FreeLibrary(lib)
#define DYNERROR
#endif
