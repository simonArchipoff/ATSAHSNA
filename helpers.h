#pragma once
#include <algorithm>
#include "constants.h"

__attribute__((unused))
static void pad_right_0(uint n, VD & in)  {
    in.resize(in.size()+n,0.0);
}

__attribute__((unused))
static void pad_left_0(uint n,VD & in){
    pad_right_0(n,in);
    std::rotate(in.rbegin(), in.rbegin() + n, in.rend());
}
__attribute__((unused))
static void remove_left(uint n, VD & in){
    std::rotate(in.begin(), in.begin() + n, in.end());
    in.resize(in.size()-n);
}
__attribute__((unused))
static void remove_right(uint n, VD & in){
    in.resize(in.size() - n);
}
__attribute__((unused))
static VCD array_VD_to_VCD(const VD & input){
    VCD out(input.size());
    for(uint i = 0; i < input.size(); i++){
        out[i] = complex<double>(input[i],0);
    }
    return out;
}

