#include "acquisition.h"
#include "signalGeneration.h"
#include <QDebug>
#include <algorithm>


VD signal(Backend * b, const struct ParamResponse p){
  switch(p.mode){
    case signal_gen_type::IMPULSE:
      return impulse(p.freqMin, p.duration,b->getSampleRate());
    case signal_gen_type::CHIRP:
      return chirp(p.freqMin,p.freqMax,p.duration,b->getSampleRate());
    }
  abort();
}


vector<struct ResultResponse> compute_response(Backend *b, const struct ParamResponse p){
  const auto in = signal(b, p);
  vector<VD> input;
  for(uint i = 0; i<b->numberInput(); i++){
      auto tmp = in;
    input.push_back(tmp);
    }
  auto output = b->acquisition(input);
  vector<struct ResultResponse> res;
  for(auto &o : output){
      auto f = compute_TF_FFT(in, o, b->getSampleRate());
      res.push_back(ResultResponse{f
                                   ,p
                                   ,MeasureData{vector({in}),vector({o})}});
    }
  return res;
}


vector<struct ResultSpectrogram> compute_spectrogram(Backend *b, const struct ParamSpectrogram p){
  auto output = compute_response(b, p);

  vector<struct ResultSpectrogram> res;
  for(auto &out_response : output){
      auto o = out_response.response.frequencyDomainTotemporal();
      auto tmp = spectrogram(o, p.nb_octave, p.resolution, b->getSampleRate());
      tmp.raw_data=out_response.raw_data;
      res.push_back(tmp);
    }
  return res;
}

vector<struct ResultTHD> compute_distortion(Backend *b, const struct ParamTHD p){
  auto in = chirp(p.frequency, p.frequency, p.duration, b->getSampleRate());

  vector<VD> input;
  for(uint i = 0; i<b->numberInput(); i++){
      input.push_back(in);
    }
  auto output = b->acquisition(input);

  vector<struct ResultTHD> res;
  for(auto &o : output){
      assert(input[0].size() == o.size());
      auto tmp = computeTHD(p, o, b->getSampleRate());
      tmp.raw_data.inputs=vector({in});
      res.push_back(tmp);
    }
  return res;
}


