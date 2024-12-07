#pragma once
#include <filesystem>
#include <unordered_map>


// Adapted from https://stackoverflow.com/questions/53530566/loading-dll-in-windows-c-for-cross-platform-design
#ifdef __unix__ || __linux__
#include <dlfcn.h>
#define DYNHANDLE void*
#define DYNOPEN(lib_path) dlopen((lib_path), RTLD_LAZY)
#define DYNSYM(lib, fn) dlsym((lib), (fn))
#define DYNCLOSE(lib) dlclose((lib))
#elif defined(_WIN32)||defined(WIN32) 
#include <Windows.h>
#define DYNHANDLE HINSTANCE
#define DYNOPEN(lib_path) LoadLibraryEx(L # lib_path)
#define DYNSYM(lib, fn) GetProcAddress((lib), (fn))
#define DYNCLOSE(lib) FreeLibrary((lib))
#endif


template<class T>
class DLMgr {
private:

public:

DLMgr() = default;
DLMgr(const DLMgr&) = delete;
DLMgr(DLMgr&&) = delete;
DLMgr& operator=(DLMgr&&) = delete;
DLMgr& operator=(const DLMgr&) = delete;

virtual ~DLMgr() = default;

};