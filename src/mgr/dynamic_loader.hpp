/**
 * @brief Dynamic loader header.

          This file defines an interface for OS-agnostic dynamic library loading, and function calling. Error handling is provided in the form of a system-agnostic error wrapper routine, with associated error code reporting.
 * @author Lee J. O'Riordan (loriordan@gmail.com)
 * @version 0.1
 * @date 2025-06-08
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <any>
#include <functional>
#include <iostream>
#include <unordered_map>

#include "system_macros.hpp"

/**
 * @brief Dynamic library loader manager.
 */
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
    auto loadFunc(const char* f_name) { 
        auto f = DYNSYM(lib_handle_, f_name); 
        return f;
    }

    virtual ~DLMgr() {
        if (lib_handle_) {
            auto ret = closeLibrary();
            if (ret) { std::cerr << DYNERROR();}
        }
    };
};

/**
 * @brief OS-specific error code to string mapper.
 * 
 * @tparam T Encoded error indicator type.
 */
template<class T>
class ErrResultT{
    private:
        bool is_success_;
        const char * err_msg_;

    public:
        ErrResultT(T result){
            #if defined(__unix__) || defined(__linux__)
            if(result>0){
                err_msg_ = dlerror();
            }
            #elif defined(_WIN32) || defined(WIN32)
            if(result){
                // Retrieve the system error message for the last-error code
                // Adapted directly from https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
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