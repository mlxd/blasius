#include <catch2/catch_test_macros.hpp>
#include "dynamic_loader.hpp"


TEST_CASE("DLMgr", "[DLMgr]") {
    SECTION("Default constructibility") {
        REQUIRE(std::is_constructible_v<DLMgr>);
    }
}