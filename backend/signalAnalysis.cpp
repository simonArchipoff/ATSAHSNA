#include "signalAnalysis.h"


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
