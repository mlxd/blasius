cmake_minimum_required(VERSION 3.14)

## CMake imports
Include(FetchContent)


project("blasius"
        VERSION 0.1.0
        DESCRIPTION "BLAS dynamic dispatch wrapper"
        LANGUAGES CXX C FORTRAN
)

add_library(blasius INTERFACE)


## Testing set-up
if(ENABLE_TESTS)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)

endif()