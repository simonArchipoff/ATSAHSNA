#include "backend.h"


#include <algorithm>




void pad_right_0(uint n, VD & in){
     in.resize(in.size()+n,0.0);
}
void remove_left(uint n, VD & in){
  std::rotate(in.begin(), in.begin() + n, in.end());
  in.resize(in.size()-n);
}
void remove_right(uint n, VD & in){
  in.resize(in.size() - n);
}

VD Latency::preprocess_input(const VD & in){
  VD res{in};
  pad_right_0(this->latency,res);
  return res;
}
vector<VD> Latency::preprocess_inputs(const vector<VD> & in){
  assert(in.size() > 0);
  vector<VD> res;
  for(auto & v : in){
      res.push_back(preprocess_input(v));
    }
  last_input = in[0];
  return res;
};
vector<VD> Latency::postprocess_output(const vector<VD> & out){
    vector<VD> res;
    switch(type){
    case KNOWN:
      for(auto & i : out){
          VD tmp = i;
          remove_left(latency,tmp);
          res.push_back(tmp);
        }
      return res;
      break;
    case UNKNOWN:
      abort();

      break;
    default:
      abort();
  }
}


#if 0
auto foo = 10000;
if(latency_automatic){
    pad_right_0(foo,in);
  }else{
    pad_right_0(l,in);
  }

    if(latency_automatic) {
        auto delay = compute_delay(out[0],in[0],10000,1024);
        setLatency(delay);
        //qDebug() << "computed latency" <<delay;
        remove_left(delay,out);
        remove_right(foo-delay,out);
      }else {
        remove_left(l,out);
      }

#endif
