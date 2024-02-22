#pragma once

#include "../constants.h"
#include "FFT.h"
#include "cassert"
#include <algorithm>
#include <numeric>
#include "../helpers.h"

enum window_type {
  RECTANGLE = 0, BOXCAR = 0 // TODO: , DIRICHLET = 0, UNIFORM = 0
  ,HANN = 1, HANNING = 1
  ,HAMMING

};



VD window(uint size, window_type t);

VD convolution_fft(const VD&a,const VD&b);
VCD convolution_fft(const VCD&a, const VCD&b);
VD correlation_fft(const VD&a, const VD&b);
VCD reverse_and_conj(const VCD & v);


template<typename T>
int compute_delay_fft(const T & s, const T & k){
  T res =  correlation_fft(s,k);
  auto m = std::max_element(res.begin(), res.end());
  auto d = m - res.begin();
  int diff = d - k.size() + 1;
  return diff;
}

int compute_delay(const VD & out, const VD & in);


template<typename iterator>
double rms(iterator begin, iterator end){
  double sq_sum = std::inner_product(begin, end, begin, 0.0,
				     [](auto const & x, auto const & y) { return x + y; },
      [](auto const & x, auto const & y) { return std::abs(x.real()*y.real()); });
  return std::sqrt(sq_sum / std::distance(begin,end));
}




template<typename T>
class ConvolutionByConstant {
  public:
  ConvolutionByConstant(const vector<complex<T>> v, uint other_operand_size)
          :ConvolutionByConstant(v.data(),v.size(), other_operand_size){

  }

  ConvolutionByConstant(const std::complex<double> *v, int size, uint other_operand_size)
      :dft(size + other_operand_size - 1)
      ,dftr(dft.getSize()){
      input.resize(dft.getSize(),0);
      for(int i = 0; i < size; i++){
          input[size - 1  - i] = std::conj(std::complex<float>(v[i].real() ,v[i].imag()));
      }
      output.resize(dft.getSize(),0);
      fft_const.resize(dft.getSize());
      dft.execute(input.data(),fft_const.data());
  }

  ConvolutionByConstant(const vector<T> & v, uint other_operand_size)
      :ConvolutionByConstant(array_VD_to_VCD(v),other_operand_size){

      };

  void convolution_fft(const T*v,int n){
      auto s = getOutput();
      for(int i = 0; i < n; i++){
        s[i] = v[i];
      }
      convolution_fft(s,n);
  }

  void convolution_fft(const complex<T> * v, int n){
      assert( n <= dft.getSize());
      std::fill(input.begin(),input.end(),0);
      std::copy(v,v+n,input.begin());
      dft.execute(input.data(),output.data());
      for(uint i = 0; i < dft.getSize() ; i++){
          input[i] = output[i] * fft_const[i];
      }
      dft.execute(input.data(),output.data());
      std::transform(getOutput(),getOutput() + getOutputSize(), getOutput(),[](std::complex<T> s){return s * std::complex<T>(1./5.,0);});
  }

  int getOutputSize(){
      return dft.getSize();
  }
  std::complex<T> * getOutput(){
      return (std::complex<T>*) output.data();
  }
  std::complex<T> * getInput(){
      return input.data();
  }

  int getSize() const{
      return dftr.getSize();
  }

  private:
  DFFT<complex<T>,complex<T>> dft;
  DFFTr<complex<T>,complex<T>> dftr;

  std::vector<std::complex<T>> fft_const
                               ,output;

  std::vector<complex<T>> input;

};


class DelayComputer
{
  private:
  DFFT<complex<float>,complex<float>> dft;
  DFFTr<complex<float>,complex<float>> dftr;
  vector<complex<float>> ref;
  vector<complex<float>> input;
  vector<complex<float>> output;
  int size_ref;
  int size_other_op;
  double refLevel;
  public:
  DelayComputer();

  /*template<typename T>
    DelayComputer(const vector<T>&v):conv(v.data(),v.size(),v.size()){}*/
  template<typename T>
  DelayComputer(const vector<T>&s, int n)
      :dft(s.size()+n-1)
      ,dftr(dft.getSize())
      ,size_ref(s.size())
      ,size_other_op(n)
      ,refLevel(1){
      ref.resize(dft.getSize());
      input.resize(dft.getSize(),0);
      output.resize(dft.getSize());
      for(uint i = 0; i < s.size(); i++){
          input[s.size() - 1 - i] = std::conj(s[i]);
      }
      dft.execute(input.data(),ref.data());
      auto r = getDelay(s.data(),s.size());
      refLevel = r.second;
  }
  //~DelayComputer();
/*«
  std::pair<int, double> getDelays(VCD &s){
      return getDelays(s.data(), s.size());
  }
*/


  std::pair<int, double> getDelay(const complex<double> * v, int n){
      for(int i= 0; i < dft.getSize(); i++){
          input[i] = i < n ? v[i].real() : 0;
      }
      return _getDelay();
    }

  std::pair<int, double> getDelay(const complex<float> * v, int n){
      for(int i= 0; i < dft.getSize(); i++){
          input[i] = i < n ? v[i].real() : 0;
      }
      return _getDelay();
  }
  std::pair<int, double> getDelay(const float * v, int n){
      for(int i= 0; i < dft.getSize(); i++){
          input[i] = i < n ? v[i] : 0;
      }
      return _getDelay();
  }
  std::pair<int, double> getDelay(const double * v, int n){
      for(int i= 0; i < dft.getSize(); i++){
          input[i] = i < n ? v[i] : 0;
      }
      return _getDelay();
  }

  protected:
  std::pair<int, double> _getDelay(){
      auto level = rms(input.begin(),input.end());
      dft.execute(input.data(),output.data());
      for(uint i = 0; i <  input.size(); i++){
          input[i]= output[i] * ref[i];
      }
      dftr.execute(input.data(),output.data());
      auto m = std::max_element(output.begin(),output.end(),[](auto a, auto b){return std::abs(a) < std::abs(b);});

      auto d = m - output.begin();
      auto lag = d - size_ref + 1;
#if 0
      vector<float> t(output.size());
      for(uint i = 0; i < t.size(); i++){
          t[i] = (std::abs(output[i]) / level) / this->refLevel;
      }
      static int foob = 0;
      std::string path = "/tmp/";
      to_file(path+"max"+std::to_string(foob)+"_" + std::to_string(lag) ,t);
      foob++;
#endif
      return std::pair{lag,((std::abs(*m)/level) / this->refLevel)};
  }
};


#if 0

ConvolutionByConstant::ConvolutionByConstant():fft_const(nullptr){}

void ConvolutionByConstant::setOperand(const VCD & v, uint other_operand_size){
    int size = v.size() + other_operand_size - 1;
    fft_const = (std::complex<double>*) fftw_alloc_complex(size);
    dft.init(size);
    auto * in = (std::complex<double> *) dft.getInput();
    std::fill(in, in+size,std::complex<double>(0,0));
    memcpy(in,v.data(), v.size() * sizeof(*in));
    VCD tmp(v);
    tmp.resize(size);
    dft.fft();
    memcpy((void*)fft_const, (void*) dft.getOutput(), dft.getSize() * sizeof(*fft_const));
}
/*
VCD ConvolutionByConstant::convolution_fft(const VCD & v){
    auto * in = (std::complex<double>*) dft.getInput();
    for(uint i = 0; i < v.size(); i++){
        in[i] = v[i];
    }
    for(uint i = v.size(); i < dft.getSize(); i++){
        in[i] = 0;
    }
    //fft
    dft.fft();
    auto * out = (std::complex<double>*) dft.getOutput();
    // multiply
    for(uint i = 0; i < dft.getSize() ; i++){
        in[i] = out[i] * fft_const[i];
    }
    dft.rfft();
    VCD o(dft.getSize());
    for(uint i = 0; i < o.size(); i++){
        o[i] = out[i]/double(dft.getSize());
    }
    return o;
}
*/


uint ConvolutionByConstant::getSize() const{
    return dft.getSize();
}



DelayComputer::DelayComputer():buff(nullptr){}
DelayComputer::~DelayComputer(){
    if(buff)
        fftw_free(buff);
}
void DelayComputer::setReference(const VCD & c){
    VCD tmp = reverse_and_conj(c);
    //tmp.resize(c.size()*2);
    conv.setOperand(tmp,c.size()*2);
    if(buff)
        fftw_free(buff);
    buff = fftw_alloc_real(conv.getOutputSize());
    this->refLevel = 1; // bit ugly hack, this allows to compute un-normalized delay
    auto r = getDelays(c);
    assert(r.first == 0);
    this->refLevel = r.second;
}

/*
std::pair<uint,double> DelayComputer::getDelays(const VCD &s){
    //return getDelays(s.begin(),s.end());
    auto tmp = conv.convolution_fft(s);
    auto r = rms(s.begin(),s.end());
    VD vd(tmp.size());
    std::transform(tmp.begin()
                   ,tmp.end()
                   ,vd.begin()
                   ,[](std::complex<double> c){return std::abs(c);});
    auto m = std::max_element(vd.begin(),vd.end());
    auto d = m-vd.begin();
    if(d - (int) s.size() + 1 < 0)
        return std::pair{0,0};
    return std::pair{d - s.size()  + 1,(*m/this->refLevel) / r};
}
*/
#endif



//https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
//thank you codeling
template<typename iterator>
double mean(iterator begin, iterator end){
  return std::accumulate(begin, end, 0.0) / std::distance(begin,end);
}
template<typename iterator>
double stddev(iterator begin, iterator end){
  double m = mean(begin,end);
  double sq_sum = std::inner_product(begin, end, begin, 0.0,
				     [](double const & x, double const & y) { return x + y; },
				     [m](double const & x, double const & y) { return (x - m)*(y - m); });
  return std::sqrt(sq_sum / std::distance(begin,end));
}
inline double mean(const VD &v){
  return mean(v.begin(),v.end());
}

inline double stddev(const VD &v){
  return stddev(v.begin(),v.end());
}

//add 2*k*pi to each sample so that it's as close as possible to the previous one
VD try_make_phase_continuous(const VD &v);

