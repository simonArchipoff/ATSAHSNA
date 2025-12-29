#include "FFT.h"
#include <cassert>



#include <vector>
#include <complex>
#include <cassert>
#include <kiss_fft.h>
#include <kiss_fftr.h>

using VF  = std::vector<float>;
using VCF = std::vector<std::complex<float>>;

VCF fft(const VF&  input, int size){
    VF ci(input);
    ci.resize(size,0);
    return fft(ci);
}
VCF fft(const VCF& input, int size){
    VCF ci(input);
    ci.resize(size,0);
    return fft(ci);
}

VCF fft(const VF& input)
{
    const int N = (int)input.size();
    const int No =  N/2 + 1;
    assert(N > 0);

    kiss_fftr_cfg cfg = kiss_fftr_alloc(N, 0, nullptr, nullptr);

    std::vector<kiss_fft_cpx> out(No);



    kiss_fftr(cfg, input.data(), out.data());
    kiss_fft_free(cfg);

    VCF result(No);
    for (int i = 0; i < No; ++i)
        result[i] = { out[i].r, out[i].i };

    return result;
}

// FFT complexe -> complexe
VCF fft(const VCF& input)
{
    const int N = (int)input.size();
    assert(N > 0);

    kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, nullptr, nullptr);

    std::vector<kiss_fft_cpx> in(N), out(N);

    for (int i = 0; i < N; ++i)
    {
        in[i].r = input[i].real();
        in[i].i = input[i].imag();
    }

    kiss_fft(cfg, in.data(), out.data());
    kiss_fft_free(cfg);

    VCF result(N);
    for (int i = 0; i < N; ++i)
        result[i] = { out[i].r, out[i].i };

    return result;
}

/* ============================================================
   FFT inverse (rfft = IFFT)
   ============================================================ */

// IFFT complexe -> réel
void rfft(const VCF& input, VF& output, int size)
{
    const int N = size / 2 + 1;
    const int No = size;
    assert(N > 0);
    assert((int)input.size() >= N);

    kiss_fftr_cfg cfg = kiss_fftr_alloc(N, 1, nullptr, nullptr);

    std::vector<kiss_fft_cpx> in(N);
    VF out(No);

    kiss_fftri(cfg, in.data(), out.data());
    kiss_fftr_free(cfg);

    const float invN = 1.0f / N;
    for (int i = 0; i < No; ++i)
        out[i] = out[i] * invN;   // partie réelle
}

// IFFT complexe -> complexe
void rfft(const VCF& input, VCF& output)
{
    const int N = (int)input.size();
    assert(N > 0);

    kiss_fft_cfg cfg = kiss_fft_alloc(N, 1, nullptr, nullptr);

    std::vector<kiss_fft_cpx> in(N), out(N);

    for (int i = 0; i < N; ++i)
    {
        in[i].r = input[i].real();
        in[i].i = input[i].imag();
    }

    kiss_fft(cfg, in.data(), out.data());
    free(cfg);

    output.resize(N);

    const float invN = 1.0f / N;
    for (int i = 0; i < N; ++i)
        output[i] = { out[i].r * invN, out[i].i * invN };
}
