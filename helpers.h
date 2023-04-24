#pragma once
#include <algorithm>
#include "constants.h"


void pad_right_0(uint n, VD & in){
    in.resize(in.size()+n,0.0);
}
void pad_left_0(uint n,VD & in){
    pad_right_0(n,in);
    std::rotate(in.rbegin(), in.rbegin() + n, in.rend());
}
void remove_left(uint n, VD & in){
    std::rotate(in.begin(), in.begin() + n, in.end());
    in.resize(in.size()-n);
}
void remove_right(uint n, VD & in){
    in.resize(in.size() - n);
}
