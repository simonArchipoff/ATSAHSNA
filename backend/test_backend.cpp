#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <cstdlib>
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


    SECTION("Write and Read Randomly Sized Sequence") {
        RingBuffer<int> fifo(std::rand()%50+1);
        int iwrite=0;
        int iread=iwrite;
        int total_read_write = 1000;
        uint available=0;

        while(total_read_write--){
            //write
            uint swrite = std::rand() % fifo.freespace();
            std::vector<int> v;
            for(uint i = 0; i < swrite; i++){
                v.push_back(iwrite++);
            }

            fifo.write(v);
            available+=swrite;
            REQUIRE(fifo.available() == available);



            uint sread = std::rand()%fifo.available();
            v = fifo.read(sread);
            for(uint i = 0; i < sread; i++){
                REQUIRE(v[i] == (iread++));
            }
            fifo.pop(sread);
            available-=sread;
            REQUIRE(fifo.available() == available);

        }
    }
}


#include <signalGeneration.h>
#include "BackendRT.h"
#include <iostream>
TEST_CASE("Acquisition") {
    Acquisition b;
    int delay=42;
    RingBuffer<double> rb(1000);
    rb.write(VD(delay));
    const uint sr = 3000;
    const uint frames = 8;
    auto foo = chirp_complex(10,1000,0.5,sr);
    //VCD foo = {1,2,3,4,0,0,0,0,0,0,0};
    b.init(sr,foo);

    b.start();

    VD in(frames);
    for(uint i = 0; i < 10000; i += frames){

        for(uint j = 0; j < frames; j++){
            if(i+j < foo.size())
                in[j] = std::real(foo[i+j])*4;
            else
                in[j] = 0;
        }


        rb.write(in);
        auto out = rb.read(frames);
        rb.pop(frames);
        auto r = b.rt_process(in, out);
        if(r.level >0.1 ){
            REQUIRE(r.idx + r.delay_result == delay);
            REQUIRE(r.level > 0.99);
        }

    }
}



