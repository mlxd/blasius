#pragma once
#include <any>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <unordered_map>


#include "system_macros.hpp"

///
/// Dynamic library loader manager
///
class DLMgr {
  private:
    DYNHANDLE lib_handle_;

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
    std::unordered_map<std::string, std::vector<FuncCallableSig<std::any>>> cfg_;


  public:
    DLMgr() = default;
    //DLMgr(const std::unordered_map<std::string, std::string>& cfg) : DLMgr() { cfg_.merge(cfg); };

    DLMgr(const DLMgr&) = delete;
    DLMgr(DLMgr&&) = delete;
    DLMgr& operator=(DLMgr&&) = delete;
    DLMgr& operator=(const DLMgr&) = delete;

    template<class T>
    DYNHANDLE loadLibrary(T lib_name);

    auto closeLibrary() {
        auto ret = DYNCLOSE(lib_handle_);
        return ret;
    }
    auto loadFunc(const char* f_name) { auto f = DYNSYM(lib_handle_, f_name); }

    virtual ~DLMgr() {
        if (lib_handle_) {
            auto ret = closeLibrary();
            if (ret) { throw std::runtime_error(DYNERROR());}
        }
    };
};

template<class T>
class ResultT{
    private:
        bool is_success_;
        char*  err_msg_;

    public:
        ResultT(T result){
            #if defined(__unix__) || defined(__linux__)
            if(result>0){
                err_msg_ = dlerror();
            }
            #elif defined(_WIN32) || defined(WIN32)
            if(result){
                // Retrieve the system error message for the last-error code
                // Adapted directly from https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
                DWORD dw = GetLastError(); 
                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    err_msg_,
                    0, nullptr);
            }
            #endif
        }
        bool is_success(){
            return is_success_;
        }
        const char* err_msg(){
            return err_msg_;
        }

};