cmake_minimum_required(VERSION 3.14)

## CMake imports
Include(FetchContent)


project("blasius"
        VERSION 0.1.0
        DESCRIPTION "BLAS dynamic dispatch wrapper"
        LANGUAGES CXX C
)

## Testing set-up
if(ENABLE_TESTS)

FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/CMake)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

endif()

## Library target declaration
add_library(blasius INTERFACE)

## Include all subdirectories
add_subdirectory(src)