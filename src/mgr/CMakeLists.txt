project(blasius_mgr)

#find_package(Catch2 REQUIRED)

add_library(blasius_mgr dynamic_loader.cpp)
target_include_directories(blasius_mgr PRIVATE ${PROJECT_SOURCE_DIR})

add_executable(blasius_mgr_tests test_dynamic_loader.cpp)
target_link_libraries(blasius_mgr_tests 
    PUBLIC blasius_mgr 
    PRIVATE Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(blasius_mgr_tests)