#pragma once

#include "../constants.h"
#include <assert.h>

VCF fft(const VF& input);
VCF fft(const VCF& input);

VCF fft(const VF&  input, int size);
VCF fft(const VCF& input, int size);

//input.size() == size / 2 + 1, output.size() == size
void rfft(const VCF & input, VF& output,int size);
//output.size() = input.size() /2 + 1
void rfft(const VCF & input, VCF& output);

#if 0
class DFFTfloat{
public:
    DFFTfloat(int size):size(size){}
    ~DFFTfloat(){
    }
    int getSize() const {
        return size;
    }
    int getOutputSize() const {
        return size;
    }

protected:
    int size;
};

class DFFTdouble{
public:
    DFFTdouble(int size):size(size){}
    ~DFFTdouble(){
    }
    int getSize() const {
        return size;
    }
    int getOutputSize() const{
        return size;
    }
    int size;
};

template<typename INTYPE, typename OUTYPE>
class DFFT{};

template<typename INTYPE, typename OUTYPE>
class DFFTr{};


template<>
class DFFT<float, std::complex<float>> : public DFFTfloat{
public:
    DFFT(int size):DFFTfloat(size){

    }
    ~DFFT(){

    }
    void execute(const float * in, std::complex<float> * out){
    }
    int getOutputSize() const{
        return size / 2 + 1;
    }

};
template<>
class DFFTr<std::complex<float>, float> : public DFFTfloat{
public:
    DFFTr(int size):DFFTfloat(size){

    }
    ~DFFTr(){

    }
    void execute(const std::complex<float> * in,float * out){
          }

};
template<>
class DFFT<std::complex<float> ,std::complex<float> > : public DFFTfloat{
public:
    DFFT(int size):DFFTfloat(size){

    }
    ~DFFT(){

    }
    void execute(const std::complex<float> * in, std::complex<float> * out){
    }
};
template<>
class DFFTr<std::complex<float>,std::complex<float>> : public DFFTfloat{
public:
    DFFTr(int size):DFFTfloat(size){

    }
    ~DFFTr(){

    }
    void execute(const std::complex<float> * in, std::complex<float> * out){
    }
};

template<>
class DFFT<double, std::complex<double>>:public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){

    }
    ~DFFT(){

    }
    void execute(double * in, std::complex<double> * out){
    }
};
template<>
class DFFT<std::complex<double>, double>:public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){

    }
    ~DFFT(){

    }
    /*
    int getOutputSize() const {
        return size/2 + 1;
    }*/
    void execute(const std::complex<double> * in, double *out){
    }
};
template<>
class DFFT<std::complex<double>, std::complex<double>> : public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){

    }
    ~DFFT(){

    }
    void execute(const std::complex<double> * in, std::complex<double> * out){
    }
};
template<>
class DFFTr<std::complex<double>, std::complex<double>> : public DFFTdouble{
public:
    DFFTr(int size):DFFTdouble(size){

    }
    ~DFFTr(){

    }
    void execute(const std::complex<double> * in, std::complex<double> * out){
    }
};


#endif 