#pragma once
#include <algorithm>
#include <fstream>
#include "constants.h"

template<typename T>
void pad_right_0(uint n, vector<T> & in)  {
    in.resize(in.size()+n,0);
}

template<typename T>
void pad_left_0(uint n, vector<T> & in){
    pad_right_0(n,in);
    std::rotate(in.rbegin(), in.rbegin() + n, in.rend());
}
template<typename T>
void remove_left(uint n, vector<T> & in){
    assert(n<=in.size());
    std::rotate(in.begin(), in.begin() + n, in.end());
    in.resize(in.size()-n);
}
template<typename T>
void remove_right(uint n, vector<T> & in){
    assert(n <= in.size());
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

__attribute__((unused))
static VD array_VCD_to_VD(const VCD & input){
    VD out(input.size());
    for(uint i = 0; i < input.size(); i++){
        out[i] = input[i].real();
    }
    return out;
}

template<typename T>
void to_file(const std::string & s, const std::vector<T> & v){
    using namespace std;
    std::ofstream out(s,ios::out | ios::binary);
    out.write((char*)v.data(), sizeof(T)*v.size());
    out.close();
}


template<typename C, typename T>
class Accumulate{
public:
    C acc;
    int size = 0;
    void reset(){
        acc = C();
        size = 0;
    }
    void add(C &v){
        if(acc.size() == 0){
            acc = v;
        } else {
            std::transform(acc.begin(),acc.end(),acc.begin(),v.end(),std::plus<T>());
        }
    }
    C get(){
        C res = acc;
        std::transform(res.begin(), res.end(), res.begin(),[this](const T & v){
            return v/T(size);
        });
        return res;
    }

};
