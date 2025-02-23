#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "helpers.h"
#include "Harmonics.h"
#include "Analysis.h"
//#include "Spectrogram.h"
#include "Generation.h"
/*
#include <fftw3.h>
TEST_CASE("fftw)"){

    double foo[20] = {0,0,
                      1,4,
                      0,0,
                      0,0};
    double bar[20] = {};
    for(int i = 0; i < 20; i++){
        bar[i] = 42;
    }

    auto p2 = fftw_plan_dft_c2r_1d(3,(fftw_complex*)foo,bar, FFTW_ESTIMATE);
    fftw_execute(p2);


    REQUIRE(1==1);
}
*/

auto a = VD({1.0, 2.0, 3.0,4.0});
auto b = VD({0.5,0.3});
auto resnumpy = VD({0.5000000000000002,1.3,2.1,2.9000000000000004,1.2000000000000002});

#if 1
TEST_CASE("Convolution FFT Test", "[convolution_fft]") {
    SECTION("Test avec des vecteurs simples") {

        double sum=0.0;
        auto result = convolution_fft(a, b);

        for(uint i = 0; i < result.size(); i++){
            sum += fabs(result[i]-resnumpy[i]);;
        };
        REQUIRE(result.size() == resnumpy.size());
        REQUIRE( sum < 0.000001 );

        BENCHMARK("bench convolution_fft") {
            return convolution_fft(a,b);
        };

    }
}
#endif
#if 0
TEST_CASE("Convolution FFT Test 2 ") {
    SECTION("Test avec des vecteurs simples") {

        double sum=0.0;

        auto a = VD({1.0, 2.0, 3.0,4.0});
        auto b = VD({0.5,0.3});
        auto resnumpy = VD({0.5000000000000002,1.3,2.1,2.9000000000000004,1.2000000000000002});

        ConvolutionByConstant  c //;;(array_VD_to_VCD(a))E

        auto result = array_VCD_to_VD(c.convolution_fft(array_VD_to_VCD(b)));


        for(uint i = 0; i < result.size(); i++){
            sum += fabs(result[i] - resnumpy[i]);;
        };
        REQUIRE(result.size() == resnumpy.size());
        REQUIRE( sum < 0.000001 );

        auto uieuie = array_VD_to_VCD(b);
        BENCHMARK("bench ConvolutionByConstant::convolution_fft") {
            return c.convolution_fft(uieuie);
        };
    }
}
#endif
TEST_CASE("computeTHD square") {
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
    REQUIRE(std::abs(100.*thd.thdRate - 48.34) < 0.01);
}


TEST_CASE("computeTHD sin") {
    VD v = sinusoid(1000,1,40000);
    ParamHarmonics p;
    p.freqMax =20000;
    p.freqMin = 1;
    p.frequency = 1000;
    double level_noise = 0.001;
    ResultHarmonics thd = computeTHD(p,v,40000);
    REQUIRE(thd.thdRate < 0.00001);
    for(int i = 0; i < v.size(); i++){
        v[i] = level_noise * (((2.0 * rand())/RAND_MAX) - 1);
    }
    auto f = rms_r(v.begin(),v.end());
    auto e_noise = level_noise / sqrt(3);
    REQUIRE(std::abs(f - e_noise) < 1e-6);
    v = sinusoid(1000,1,40000);
    for(int i = 0; i < v.size(); i++){
        v[i] += level_noise * (((2.0 * rand())/RAND_MAX) - 1);
    }
    double thdntheoric = e_noise / (1/sqrt(2));
    thd = computeTHD(p,v,40000);
    REQUIRE(std::abs(thd.thdNoiseRate - thdntheoric)  < 1e-6);
}


TEST_CASE("chirp"){
    int SR = 44000;
    double freq = 100;
    double d = 0.1;
    auto s = sinusoid(freq,d,SR);
    auto f = compute_TF_FFT(array_VD_to_VCD(s), SR);
    auto a = f.getAmplitude20log10();
    REQUIRE(a[freq * d] == *std::max_element(a.begin(),a.end()));
}


TEST_CASE("find delay fft","[benchmark]"){
    int SR = 44000;
    double d = 0.1;
    auto o = chirp(100,10000,d,SR);
    auto s{o};
    pad_right_0(1501,s);

    int delay = 666;
    pad_left_0(delay,s);


    int diff = compute_delay_fft(s,o);

    REQUIRE(diff == delay);
}



TEST_CASE("find delay DelayComputer","[benchmark]"){
    int SR = 44000;
    double d = 0.1;
    auto o = chirp(100,10000,d,SR);
    auto s{o};
    pad_right_0(1501,s);

    int delay = 666;
    pad_left_0(delay,s);
    DelayComputer dc(array_VD_to_VCD(o),s.size());


    vector<float> f(s.begin(), s.end());
    auto p = dc.getDelay(f.data(),f.size());

    int diff = p.first;


    /*
    to_file("/tmp/s",s);
    to_file("/tmp/o",o);
    to_file("/tmp/res",res);
*/
    REQUIRE(diff == delay);
}



TEST_CASE("optimal window"){
    int SR = 44000;
    double f = 100;
    auto s = sinusoid(f,0.1,SR);
    auto signal = s;
    pad_right_0(100,signal);
    pad_left_0(256,signal);
    VD r = optimal_window(signal,f,SR);
    REQUIRE(s == r);
}

#if 0
TEST_CASE("stft"){
    int SR = 44000;
    double d = 0.1;
    auto o = chirp(10,22000,d,SR);

    auto res = stft(o.data(), o.data() + o.size(), 1024, 32, SR, HANN);

    BENCHMARK("stft") {
        return stft(o.data(), o.data() + o.size(), 1024, 1, SR, HANN);
    };

    //to_file("/tmp/stft",res.data);

}

#endif
