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

template<typename T>
vector<complex<T>> array_VD_to_VCD(const vector<T> & input){
    VCD out(input.size());
    for(uint i = 0; i < input.size(); i++){
        out[i] = complex<T>(input[i],0);
    }
    return out;
}

template<typename T, typename U>
vector<T> array_complex_to_real(const vector<complex<U>> & s){
    vector<T> r;
    r.resize(s.size());
    for(uint i = 0; i < r.size(); i++){
        r[i] = s[i].real();
    }
    return r;
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
    std::ofstream out(s, ios::out | ios::binary);
    out.write((char*)v.data(), sizeof(T)*v.size());
    out.close();
}
template<typename T>
void to_file(const std::string & s, const T*p, int size){
    std::vector<T> v(p,p+size);
    return to_file(s,v);
}
template<typename T, typename iterator>
void to_file(const std::string &s, iterator begin, iterator end){
    std::vector<T> v(begin,end);
    return to_file(s,v);
}
template<typename T>
void to_file(const std::string &s, const T*begin, const T*end){
    std::vector<T> v(begin,end);
    return to_file(s,v);
}

template<typename T>
void append_to_file(const std::string &s, const T * p, int size){
    using namespace std;
    std::ofstream out(s, ios::out | ios::binary | ios::app);
    out.write((char*)p, sizeof(T)*size);
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
            assert(v.size() == acc.size());
            for(uint i = 0 ; i < acc.size(); i++){
                acc[i] += v[i];
            }
        }
        size++;
    }
    C get(){
        C res(acc.size());
        for(uint i = 0; i < res.size(); i++){
            res[i] = acc[i] / T(size);
        }
        return res;
    }
    int getNumberAcc(){
        return size;
    }

};


template<>
struct Accumulate<double,double>{
    double acc = 0;
    int size = 0;
    void reset(){
        acc = 0.0;
        size = 0;
    }
    void add(double d){
        acc += d;
        size++;
    }
    double get(){
        return acc/size;
    }

};
