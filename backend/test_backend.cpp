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

//tests made by chatGPT (and fixed by me)

TEST_CASE("RingBuffer - Basic Operations", "[RingBuffer]") {
    SECTION("Default Constructor") {
        RingBuffer<int> rb;
        REQUIRE(rb.available() == 0);
        REQUIRE(rb.freespace() == 0);
    }

    SECTION("Constructor with Size") {
        RingBuffer<int> rb(10);
        REQUIRE(rb.available() == 0);
        REQUIRE(rb.freespace() == 10);
    }

    SECTION("Reset Buffer Size") {
        RingBuffer<int> rb;
        rb.reset(5);
        REQUIRE(rb.available() == 0);
        REQUIRE(rb.freespace() == 5);
    }

    SECTION("Write and Read Elements") {
        RingBuffer<int> rb(5);
        std::vector<int> data = {1, 2, 3};
        rb.write(data);
        REQUIRE(rb.available() == 3);
        REQUIRE(rb.freespace() == 2);

        std::vector<int> readData = rb.read(2);
        REQUIRE(readData == std::vector<int>({1, 2}));
        REQUIRE(rb.available() == 3);
        REQUIRE(rb.freespace() == 2);

        rb.write({4, 5});
        REQUIRE(rb.available() == 5);
        REQUIRE(rb.freespace() == 0);

        readData = rb.read(4);
        REQUIRE(readData == std::vector<int>({1, 2, 3,4}));
        REQUIRE(rb.available() == 5);
        REQUIRE(rb.freespace() == 0);
    }

    SECTION("Pop Elements") {
        RingBuffer<int> rb(5);
        rb.write({1, 2, 3, 4, 5});
        REQUIRE(rb.available() == 5);
        REQUIRE(rb.freespace() == 0);

        rb.pop(2);
        REQUIRE(rb.available() == 3);
        REQUIRE(rb.freespace() == 2);

        std::vector<int> readData = rb.read(2);
        REQUIRE(readData == std::vector<int>({3, 4}));
        REQUIRE(rb.available() == 3);
        REQUIRE(rb.freespace() == 2);

        rb.pop(1);
        REQUIRE(rb.available() == 2);
        REQUIRE(rb.freespace() == 3);
    }
}

#include <signalGeneration.h>
#include "BackendRT.h"
TEST_CASE("BackendResponse") {
    BackendResponse b;
    int delay=142;
    RingBuffer<float> rb(delay);
    rb.write(VF(142));

    auto foo = chirp_complex(10,1000,1,3000);

    b.init(3000,foo);
    VCD bar(foo);
    b.start();

    const uint frames = 32;
    VF in(frames);
    for(uint i = 0; i < 10000; i += frames){
        VF out(32);
        b.process(frames,in.data(), rb.read(frames).data());
        rb.write(in);
    }
}



