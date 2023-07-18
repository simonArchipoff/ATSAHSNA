#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "backendRingBuffer.h"
#include <vector>

TEST_CASE("Ring buffer") {
    RingBuffer<int> rb(8);


    rb.write({1,2,3,4,5});
    REQUIRE(rb.available() == 5);
    auto foo = rb.read(3);

    REQUIRE(foo[0] == 1);
    REQUIRE(foo[1] == 2);
    REQUIRE(foo[2] == 3);

    rb.pop(3);
    REQUIRE(rb.available() == 2);
    rb.write({6,7,8,9});
    REQUIRE(rb.available() == 6);
    auto bar = rb.read(rb.available());
    REQUIRE(bar == std::vector({4,5,6,7,8,9}));
    rb.write({10,11});
    REQUIRE(rb.available() == 8);

}
