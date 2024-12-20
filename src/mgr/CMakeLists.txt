project(blasius_mgr)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests test_dynamic_loader.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)