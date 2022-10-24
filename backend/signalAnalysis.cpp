#include "signalAnalysis.h"
#include <numeric>
#include <QDebug>

#include <fstream>
#include <iomanip>


VD convolution(const VD &v, uint start, uint size,
               const VD &k, uint kstart, uint ksize){
  VD res(size+ksize);
  for(uint i = 0; i < size; i++){
      for(uint j = 0; j < ksize; j++){
          res[i] += v[start + i + j] * k[kstart + j];
        }
    }
  return res;
}


void find_maximums(const VD & in, vector<int> & idx, vector<double> & maxs, double minimum_max){
  idx.resize(0);
  maxs.resize(0);
  if(in.size() > 1 && in[0] > in[1] && in[0] > minimum_max){ //is first element a maximum?
      idx.push_back(0);
      maxs.push_back(in[0]);
    }
  for(int i = 1; static_cast<size_t>(i) < in.size() -1 ; i++){
      if(in[i-1] < in[i] && in[i] > in[i+1] && in[i] > minimum_max){
          idx.push_back(i);
          maxs.push_back(in[i]);
        }
    }
  if(in.size() > 2 && in[in.size() - 2] < in[in.size() -1] && in[in.size() -1] > minimum_max){ //is last element a maximum?
      idx.push_back(in.size() -1);
      maxs.push_back(in[in.size() -1]);
    }
}


int compute_delay(const VD & out, const VD & in, int maximum_delay,int size_input){
  VD k(2*size_input);
  for(uint i = 0; i < size_input; i++){
      k[i] = in[size_input - i - 1];
      k[size_input + i] = in[i];
    }
  auto conv_out = convolution(out,0,maximum_delay + size_input, k, 0, k.size());
  std::ofstream outFile("/tmp/foo");
  for (const auto &e : conv_out)
    outFile <<  std::setprecision(17) << std::setw(25) << e << "\n";
  for(auto & i : conv_out)
    i = abs(i);
  double m = *std::max_element(conv_out.begin(), conv_out.end());
  vector<int> idx;
  vector<double> maxs;
  find_maximums(conv_out, idx, maxs,0.99*m);
  qDebug() << idx;
  return idx[0] + size_input;
}


//https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
//thank you codeling
double mean(const VD &v){
  return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}
double stddev(const VD & v){
    double m = mean(v);
    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0,
        [](double const & x, double const & y) { return x + y; },
        [m](double const & x, double const & y) { return (x - m)*(y - m); });
    return std::sqrt(sq_sum / v.size());
}


VD try_make_phase_continuous(const VD &v){
  vector<double> res = VD{v};
  for(uint i = 1; i < v.size() ; i++){
        auto prev = res[i-1];
        int k = static_cast<int>(std::round((prev-v[i])/360.));
        res[i] = res[i] + k * 360.;
      }
  return res;
}




