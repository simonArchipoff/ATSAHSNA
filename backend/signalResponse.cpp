#include "signalResponse.h"



FDF computeResponse(const VD & input, const VD & output, int sampleRate){
  return compute_TF_FFT(input,output,sampleRate);
}


