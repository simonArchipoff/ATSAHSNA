#pragma once

#include "../constants.h"
#include "signalFrequencyDomain.h"

class Latency {
public:
  VD preprocess_input(const VD & in);
  vector<VD> preprocess_inputs(const vector<VD> & in);
  vector<VD> postprocess_output(const vector<VD> & out);
  void setLatency(int l){
    type = KNOWN;
    latency = l;
  }
  void setUnknownLatency(int max_latency){
    type = UNKNOWN;
    latency = max_latency;
  }
  enum Latency_Type {UNKNOWN, KNOWN};
  int latency = 0;
  Latency_Type type = KNOWN;
  VD last_input;
};


class Backend {
public:
  virtual ~Backend() = default;
  virtual uint numberInput() const = 0;
  virtual uint numberOutput() const = 0;
  virtual uint getSampleRate() const = 0;
  virtual bool isReady() const = 0;
  //virtual int latency() const {return 0;}
  virtual vector<VD> acquisition(const vector<VD> &input) = 0;

  //ComputeImpulseResponse * compute_impulse = nullptr;
  //ComputeSinResponse * compute_sin = nullptr;
  Latency latency;
};

template<class C>
vector<typename C::Result> compute(Backend * b, typename C::Param p){
  auto input =
      b->latency.preprocess_input(C{}.generate_data(p, b->getSampleRate()));
  vector<VD> inputs;
  for(uint i = 0; i < b->numberInput() ; i++)
    inputs.push_back(input);
  auto outputs = b->latency.postprocess_output(b->acquisition(inputs));
  vector<typename C::Result> res;
  for(auto & o : outputs){
      res.push_back(C{}.computeResult(o,p,b->getSampleRate()));
    }
  return res;
}
