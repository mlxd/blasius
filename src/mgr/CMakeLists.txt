project(blasius_mgr)


add_executable(tests test_dynamic_loader.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)