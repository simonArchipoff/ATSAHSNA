#pragma once

#include "../constants.h"

struct MeasureData{
    int numberInput(){
        return inputs.size();
    }
    int numberOutput(){
        return outputs.size();
    }

    vector<VD> inputs;
    vector<VD> outputs;
};


enum window_type {
    RECTANGLE = 0, BOXCAR = 0, DIRICHLET = 0, UNIFORM = 0
    ,HANN = 1, HANNING = 1
    ,HAMMING
};



VD window(uint size, window_type t);

//naive implementation for finding k in v
VD correlation(const VD &v, uint start, uint size,
               const VD &k, uint kstart, uint ksize);
int compute_delay(const VD & out, const VD & in);

double mean(const VD &v);
double stddev(const VD &v);

//add 2*k*pi to each sample so that it's as close as possible to the previous one
VD try_make_phase_continuous(const VD &v);

