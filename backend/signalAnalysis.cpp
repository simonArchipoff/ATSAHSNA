#include "signalAnalysis.h"
#include <numeric>

VD convolution(const VD &v, const VD&c){
  VD res(v.size()-c.size());
  for(uint i = 0; i < v.size() - c.size(); i++){
      res[i] = 0;
      for(uint j = 0; j < c.size(); j++){
          res[i] += v[i+j] * c[j];
        }
    }
  return res;
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
