#include "Response.h"
#include "Generation.h"


ResultResponse computeResponse(ParamResponse p,
                               const VD & in,
                               const VD & out,
                               uint sampleRate){
    return {.response = compute_TF_FFT(in,out,sampleRate)
            ,.params = p};
}
ResultResponse computeResponse(ParamResponse p, const VD&out, uint sampleRate){
    return {.response = compute_TF_FFT(out,sampleRate)
            ,.params = p};
}


VCD computeChirp(ParamResponse p, uint sampleRate){
    assert(sampleRate > 0);
    double dur = 1./p.freqMin;
    assert(dur > 0);
    return chirp_complex(p.freqMin,p.freqMax,std::max(2*dur,p.duration),sampleRate);
}
