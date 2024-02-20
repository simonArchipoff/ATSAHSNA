

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



class TestRTModuleBackend : public BackendFaust, public RTModuleHandler {
public:

    TestRTModuleBackend():BackendFaust(std::string("foo")){
        setCode("process = @(10);",44100);
        RTModuleHandler::setSampleRate(44100);
    }

    void run(){
        AudioIO<float> in,out;
        vector<float> inv,outv;
        const int SIZE = 32;
        inv.resize(SIZE);
        outv.resize(SIZE);

        in.addChannel(SIZE,inv.data());
        out.addChannel(SIZE,outv.data());

        for(int x = 0; x <1024 ; x++){

            rt_process(in,out);
            vector<double> ind;
            ind.resize(SIZE);
            for(int i = 0; i < SIZE; i++){
                ind[i] = out[0][i];
            }
            auto outd = acquisition(vector<VD>({ind}));

            for(int i = 0; i < SIZE; i++){
                in[0][i] = outd[0][i];
            }
        }
    }
};

TEST_CASE("RTModuleHandler") {
    TestRTModuleBackend rtm;
    ParamResponse p;
    p.duration=0.01;
    p.freqMin = 5000;
    p.freqMin=2000;
    rtm.startResponse(p,1,1);
    rtm.run();
    REQUIRE(false);
}
