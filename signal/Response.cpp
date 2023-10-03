#include "Response.h"
#include "Generation.h"


ResultResponse computeResponse(ParamResponse p,
                               const VD & in,
                               const VD & out,
                               uint sampleRate){
    return {.response = compute_TF_FFT(in,out,sampleRate)
            ,.params = p};
}


VCD computeChirp(ParamResponse p, uint sampleRate){
    double dur = 1./p.freqMin;
    return chirp_complex(p.freqMin,p.freqMax,2*dur,sampleRate);
}
