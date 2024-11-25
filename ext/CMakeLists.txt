include(FetchContent)
if(DEFINED USE_REF_LAPACK)
    FetchContent_Declare(
    lapack
    GIT_REPOSITORY https://github.com/Reference-LAPACK/lapack.git
    GIT_TAG        v3.12.0
    )

    FetchContent_MakeAvailable(lapack)
end()