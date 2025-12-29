
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <cstdlib>
#include "RingBuffer.h"

#include <vector>
#include <Faust.h>
#include <Sender.h>
#include <QString>
#include <QCoreApplication>



#if 1
TEST_CASE("Ring buffer") {
    RingBuffer<int> rb(8);


    rb.write({1,2,3,4,5});
    REQUIRE(rb.available() == 5);
    std::vector<int> v;
    v.resize(3.0);
    rb.read(3,v.data());

    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);

    rb.pop(3);
    REQUIRE(rb.available() == 2);
    rb.write({6,7,8,9});
    REQUIRE(rb.available() == 6);
    v.resize(rb.available());
    rb.read(rb.available(),v.data());
    REQUIRE(v == std::vector({4,5,6,7,8,9}));
    rb.write({10,11});
    REQUIRE(rb.available() == 8);

}
#endif
//tests made by chatGPT (and fixed by me)

#if 1
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

        std::vector<int> readData;
        readData.resize(2);
        rb.read(2,readData.data());
        REQUIRE(readData == std::vector<int>({1, 2}));
        REQUIRE(rb.available() == 3);
        REQUIRE(rb.freespace() == 2);

        rb.write({4, 5});
        REQUIRE(rb.available() == 5);
        REQUIRE(rb.freespace() == 0);

        readData.resize(4);
        rb.read(4,readData.data());
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

        std::vector<int> readData;
        readData.resize(2);
        rb.read(2,readData.data());
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
            v.resize(sread);
            fifo.read(sread,v.data());
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
    rb.write(VF(delay));
    const uint sr = 3000;
    const uint frames = 256;
    auto foo = chirp_complex(10,1000,0.5,sr);
    //VCF foo = {1,2,3,4,0,0,0,0,0,0,0};
    Acquisition b(foo,SenderMode::All,1,0,1,100);

    //b.start();

    VF in(frames);
    int res_delay = -1;
    for(uint i = 0; i < 10000; i += frames){

        for(uint j = 0; j < frames; j++){
            if(i+j < foo.size())
                in[j] = std::real(foo[i+j])*4;
            else
                in[j] = 0;
        }


        rb.write(in);
        std::VF out;
        out.resize(frames);
        rb.read(frames,out.data());
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

#if 1
TEST_CASE("Test de Sender") {
    VF signal = {1.0, 2.0, 3.0, 4.0};
    int number = 33;
    int timeoff = 2;
    Sender sender(signal, SenderMode::All, number, timeoff);

    SECTION("Test de rt_send") {

        vector<float> output;
        output.resize(number * (timeoff + signal.size()), 42);

        int nb_frames = 5;
        int nb_call = output.size() / nb_frames;

        for(int i = 0; i < nb_call; i++){
            float * t = output.data() + i * nb_frames;
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


#include <RTModule.h>


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
        auto r = BackendFaust::acquisition(std::vector<VF>({input}));


        vector<float> inv(size),outv(size);

        AudioIO<float> in,out;
        in.addChannel(size,inv.data());
        out.addChannel(size,outv.data());

        for(int i = 0; i < size; i++){
            inv[i] = r[0][i];
        }

        RTModuleHandler::rt_process(in,out);
        for(int i = 0; i < size; i++){
            input[i] = outv[i];
        }

        RTModuleHandler::rt_after_process();
    }

public:
    int size;
    VF input;

};

#if 0

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
