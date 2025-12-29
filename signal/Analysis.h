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



VF window(uint size, window_type t);

VF convolution_fft(const VF&a,const VF&b);
VCF convolution_fft(const VCF&a, const VCF&b);
VF correlation_fft(const VF&a, const VF&b);
VCF reverse_and_conj(const VCF & v);


template<typename T>
int compute_delay_fft(const T & s, const T & k){
  T res =  correlation_fft(s,k);
  auto m = std::max_element(res.begin(), res.end());
  auto d = m - res.begin();
  int diff = d - k.size() + 1;
  return diff;
}

int compute_delay(const VF & out, const VF & in);


template<typename iterator>
double rms(const iterator begin, const iterator end){
  double sq_sum = std::inner_product(begin, end, begin, 0.0,
				     [](auto const & x, auto const & y) { return x + y; },
      [](auto const & x, auto const & y) { return std::abs(x.real()*y.real()); });
  return std::sqrt(sq_sum / std::distance(begin,end));
}
template<typename iterator>
inline double rms_r(const iterator begin, const iterator end){
  double sq_sum = std::inner_product(begin, end, begin, 0.0,
      [](auto const & x, auto const & y) { return x + y; },
      [](auto const & x, auto const & y) { return std::abs(x*y); });
  return std::sqrt(sq_sum / std::distance(begin,end));
}




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
inline double mean(const VF &v){
  return mean(v.begin(),v.end());
}

inline double stddev(const VF &v){
  return stddev(v.begin(),v.end());
}

//add 2*k*pi to each sample so that it's as close as possible to the previous one
VF try_make_phase_continuous(const VF &v);

