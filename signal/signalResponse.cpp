#include "signalResponse.h"
#include "signalGeneration.h"

VD ImpulseResponse::generate_data(Param p, uint sampleRate){
    return chirp(p.freqMin, p.freqMax, 100./p.freqMin, sampleRate);
}


ResultResponse ImpulseResponse::computeResult(const VD & out ,
                                              ParamResponse p,
                                              uint sampleRate){
    auto i = generate_data(p,sampleRate);
    return {.response = compute_TF_FFT(i,out,sampleRate)
            ,.params = p};
}
