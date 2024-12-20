#pragma once
#include <filesystem>
#include <unordered_map>

// Adapted from
// https://stackoverflow.com/questions/53530566/loading-dll-in-windows-c-for-cross-platform-design
#ifdef __unix__ || __linux__
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

///
/// Dynamic library loader manager
///
class DLMgr {
  private:
    DYNHANDLE lib_handle_;
    std::unordered_map<std::string, std::vector> cfg_;

    // Follows
    // https://stackoverflow.com/questions/45715219/store-functions-with-different-signatures-in-a-map
    template <class RType>
    class FuncCallableSig {
      private:
        std::any m_any_;

      public:
        FuncCallableSig() = default;

        template <class F>
        FuncCallableSig(F&& func) : FuncCallableSig(std::function(std::forward<F>(func))){};

        template <class... Args>
        FuncCallableSig(std::function<RType(Args...)> func) : m_any_(func) {}

        template <class... Args>
        RType operator()(Args&&... args) {
            return std::invoke(std::any_cast<std::function<RType(Args...)>>(m_any_), std::forward<Args>(args)...);
        }
    };

  public:
    DLMgr() = default;

    DLMgr(const std::unordered_map<std::string, std::string>& cfg = {}) : DLMgr() { cfg_.merge(cfg); };

    DLMgr(const DLMgr&) = delete;
    DLMgr(DLMgr&&) = delete;
    DLMgr& operator=(DLMgr&&) = delete;
    DLMgr& operator=(const DLMgr&) = delete;

    DYNHANDLE loadLibrary(const char* lib_name) {
#ifdef __unix__ || __linux__
        auto local_cfg = cfg_["dlopen"];
        lib_handle_ = DYNOPEN(lib_name, );
#else
        auto local_cfg = cfg_["LoadLibraryEx"];
        lib_handle_ = DYNOPEN(lib_name);
#endif
    }
    auto closeLibrary() {
        auto ret = DYNCLOSE(lib_handle_);
        return ret;
    }
    auto loadFunc(const char* f_name) { auto f = DYNSYM(lib_handle_, f_name); }

    virtual ~DLMgr() {
        if (lib_handle_) {
            auto ret = closeLibrary();
            if (ret) { throw std::exception(DYNERROR()); }
        }
    };
};