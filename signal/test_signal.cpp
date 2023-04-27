#include <catch2/catch_test_macros.hpp>
#include "helpers.h"
#include "signalHarmonics.h"
#include "signalGeneration.h"
#include "signalAnalysis.h"

TEST_CASE("computeTHD") {
    uint t = 2*2048;
    VD v(t);
    for(uint i = 0; i < v.size(); i++){
        v[i] = i < t/2 ? 1 : -1;
    }
    ParamHarmonics p;
    p.freqMax = t/2;
    p.freqMin = 1;
    p.frequency = 1;
    ResultHarmonics thd = computeTHD(p,v,t);
    REQUIRE(std::abs(thd.thdRate - 48.34) < 0.01);
}


TEST_CASE("chirp"){
    int SR = 44000;
    double freq = 100;
    double d = 0.1;
    auto s = chirp(freq,freq,d,SR);
    auto f = compute_TF_FFT(array_VD_to_VCD(s), SR);
    auto a = f.getAmplitude20log10();
    REQUIRE(a[freq * d] == *std::max_element(a.begin(),a.end()));
}


TEST_CASE("find delay"){
    int SR = 44000;
    double d = 0.1;
    auto o = chirp(100,10000,d,SR);
    auto s{o};
    pad_right_0(1500,s);

    pad_left_0(530,s);

    int delay = compute_delay(s, o);

    REQUIRE(delay == 530);

}


