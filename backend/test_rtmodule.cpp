

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>


//#include "Harmonics.h"
#include "Faust.h"
#include <cstring>

#include "RTModule.h"

#include <mutex>
#include <variant>
#include <optional>
#include <string.h>
#include <iostream>



class TestRTModuleBackend :  public RTModuleHandler {
public:

    TestRTModuleBackend(){
        b = new BackendFaust(std::string("foo"));
        b->setCode("import(\"filters.lib\");\nprocess = @(1536);",48000);
        RTModuleHandler::setSampleRate(48000);
    }
    ~TestRTModuleBackend(){
        delete b;
    }
    void run(){
        AudioIO<float> in,out;
        vector<float> inv,outv;
        const int SIZE = 1024*8;
        inv.resize(SIZE);
        outv.resize(SIZE);

        in.addChannel(SIZE,inv.data());
        out.addChannel(SIZE,outv.data());

        for(int x = 0; x <48000*50 / SIZE ; x++){

            rt_process(in,out);
            vector<double> ind;
            ind.resize(SIZE);
            for(int i = 0; i < SIZE; i++){
                ind[i] = out[0][i];
            }
            auto outd = b->acquisition(vector<VD>({ind}));

            for(int i = 0; i < SIZE; i++){
                in[0][i] = outd[0][i];
            }
        }
    }
    BackendFaust * b;
};

TEST_CASE("RTModuleHandler") {
    {
    TestRTModuleBackend rtm;
    ParamResponse p;
    p.duration=0.01;
    p.freqMin = 20;
    p.freqMax = 20000;
    rtm.startResponse(p,1,2);

    rtm.run();
    vector<ResultResponse> r1,r2,r3,r4,r5;
    auto a = rtm.getResultResponse(r1);
    auto b = rtm.getResultResponse(r2);
    auto c = rtm.getResultResponse(r3);
    auto d = rtm.getResultResponse(r4);
    auto e = rtm.getResultResponse(r5);
    }
    REQUIRE(true);
}
