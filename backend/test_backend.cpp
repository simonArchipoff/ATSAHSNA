
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <cstdlib>
#include "RingBuffer.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <Faust.h>
#include <Sender.h>
#if 0
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
#endif
//tests made by chatGPT (and fixed by me)

#if 0
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

#endif

#if 0
#include <Generation.h>
#include <Acquisition.h>
#include <iostream>

TEST_CASE("Acquisition") {
    int delay=45;
    RingBuffer<double> rb(10000);
    rb.write(VD(delay));
    const uint sr = 3000;
    const uint frames = 256;
    auto foo = chirp_complex(10,1000,0.5,sr);
    //VCD foo = {1,2,3,4,0,0,0,0,0,0,0};
    Acquisition b(foo,SenderMode::All,1,0,1,100);

    //b.start();

    VD in(frames);
    int res_delay = -1;
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
        auto r = b.rt_process(out, in);

        try {
            auto res = std::get<Acquisition::result>(r);
            REQUIRE(res_delay == -1);
            res_delay = res.delay;
        } catch (const std::bad_variant_access& ex){

        }

        try {
            auto res = std::get<Acquisition::timeout>(r);
        } catch (const std::bad_variant_access& ex){

        }
    }
    REQUIRE(res_delay == delay);
}
#endif

#if 0
TEST_CASE("Test de Sender") {
    VF signal = {1.0, 2.0, 3.0, 4.0,6.0,7.0,8.0,9.0,10.,11.,12.,13.,14.,15,16.};
    int number = 33;
    int timeoff = 1;
    Sender sender(signal, SenderMode::All, number, timeoff);

    SECTION("Test de rt_send") {

        float output[number * (timeoff + signal.size())];

        memset(output,42,sizeof(output));

        int nb_frames = 5;
        int nb_call = (sizeof(output)/sizeof(output[0])) / nb_frames;

        for(int i = 0; i < nb_call; i++){
            float * t = output + i * nb_frames;
            AudioIO<float> o(nb_frames,1,&t);
            sender.rt_output(o);
        }

        REQUIRE(output[0] == 0.0);
        REQUIRE(output[1] == 0.0);
        REQUIRE(output[2] == 1.0);
        REQUIRE(output[3] == 2.0);
        REQUIRE(output[4] == 3.0);
        REQUIRE(output[5] == 4.0);

        REQUIRE(output[6] == 0.0);
        REQUIRE(output[7] == 0.0);

        REQUIRE(output[8] == 1.0);
        REQUIRE(output[9] == 2.0);
        REQUIRE(output[10] == 3.0);
        REQUIRE(output[11] == 4.0);

        REQUIRE(output[12] == 0.0);
        REQUIRE(output[13] == 0.0);

        REQUIRE(output[14] == 1.0);
        REQUIRE(output[15] == 2.0);
        REQUIRE(output[16] == 3.0);
        REQUIRE(output[17] == 4.0);

        REQUIRE(output[18] == 0.0);
        REQUIRE(output[19] == 0.0);

        //REQUIRE(output[20] == 0.0);
        //REQUIRE(output[21] == 0.0);

    }
}
#endif
#if 0

class FaustWithRTModule : public BackendFaust, public RTModuleHandler{
public:
    FaustWithRTModule(int s):BackendFaust("test"),size(s){}

    void setCode(string code, int sr){
        BackendFaust::setCode(code,sr);
        RTModuleHandler::setSampleRate(sr);
    }

    void run1cycle(){
        if(input.empty()){
            input.resize(size);
        }
        auto r = BackendFaust::acquisition(std::vector<VD>({input}));
        vector<VD> foo({input});
        RTModuleHandler::rt_process(vector<VD>(r),foo);
        input = foo[0];
        RTModuleHandler::rt_after_process();
    }

public:
    int size;
    VD input;

};



TEST_CASE("Test RTModuleHandler 1"){
    const int sr = 20;
    const int vector_size = 16;
    const int delay = 23;
    FaustWithRTModule f(vector_size);
    f.setCode("process = _ @" + std::to_string(delay) +";\n",sr);
    ParamResponse p{1, 5, 1};
    f.startResponse(p,0,4);
    for(int i = 0; i < 10000; i+=vector_size){
           f.run1cycle();
    }
    vector<ResultResponse> r;
    if(f.RTModuleHandler::getResultResponse(r)){
           1;
    }
}

#endif
