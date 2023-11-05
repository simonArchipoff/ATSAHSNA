#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <concepts>
#include <cstdint>
#include "constants.h"
#include "helpers.h"


TEST_CASE( "VectorHelpers", "[helpers]" ){
    VD v{1,2};
    auto v1{v};
    int n = 3;
    pad_right_0(n,v1);
    REQUIRE( v1 == VD{1,2,0,0,0});
    VD v2{v};
    pad_left_0(n,v2);
    REQUIRE(v2 == VD{0,0,0,1,2});
    remove_left(n,v2);
    REQUIRE(v2==v);
    remove_right(n,v1);
    REQUIRE(v1==v);
}

