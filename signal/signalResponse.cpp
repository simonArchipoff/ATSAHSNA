#include "signalResponse.h"
#include "signalGeneration.h"


ResultResponse computeResponse(ParamResponse p,
                               const VD & in,
                               const VD & out,
                               uint sampleRate){
    return {.response = compute_TF_FFT(in,out,sampleRate)
            ,.params = p};
}
