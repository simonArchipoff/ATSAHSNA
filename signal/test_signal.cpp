#include <catch2/catch_test_macros.hpp>

#include "signalHarmonics.h"

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
