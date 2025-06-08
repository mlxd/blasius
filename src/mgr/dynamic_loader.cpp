#include "dynamic_loader.hpp"

template<>
inline DYNHANDLE DLMgr::loadLibrary(const char* lib_name){
#if defined(__unix__) || defined(__linux__)
        auto local_cfg = DLMgr::cfg_["dlopen"];
        lib_handle_ = DYNOPEN(lib_name, local_cfg["flags"]);
#else
        auto local_cfg = cfg_["LoadLibraryEx"];
        lib_handle_ = DYNOPEN(lib_name);
#endif
}
