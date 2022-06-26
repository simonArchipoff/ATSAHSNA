#include "acquisition.h"
#include "signalGeneration.h"
#include <QDebug>
#include <algorithm>

vector<struct ResultResponse> compute_response(Backend *b, const struct ParamResponse p){
  const auto in = impulse(p.freqMin,b->getSampleRate());
  vector<VD> input;
  for(uint i = 0; i<b->numberInput(); i++){
    input.push_back(in);
    }
  auto output = b->acquisition(input);
  vector<struct ResultResponse> res;
  for(auto &o : output){
      res.push_back(ResultResponse{ compute_TF_FFT(in,o,b->getSampleRate())
                                   ,p
                                   ,MeasureData{vector({in}),vector({o})}});
    }
  return res;
}

vector<struct ResultSpectrogram> compute_spectrogram(Backend *b, const struct ParamSpectrogram p){
  const auto in = impulse(1,b->getSampleRate());
  vector<VD> input;
  for(uint i = 0; i<b->numberInput(); i++){
    input.push_back(in);
    }
  auto output = b->acquisition(input);
  vector<struct ResultSpectrogram> res;
  for(auto &o : output){
      auto tmp = spectrogram(o,p.nb_octave,p.resolution,b->getSampleRate());
      tmp.raw_data.inputs=vector({in});
      tmp.raw_data.outputs=vector({o});
      res.push_back(tmp);
    }
  return res;
}

vector<struct ResultTHD> compute_distortion(Backend *b, const struct ParamTHD p){
  auto in = sweep(p.frequency,p.frequency,p.duration, b->getSampleRate());
  for(auto &i:in)
    i/=2;
  vector<VD> input;
  for(uint i = 0; i<b->numberInput(); i++){
      input.push_back(in);
    }
  auto output = b->acquisition(input);

  vector<struct ResultTHD> res;
  for(auto &o : output){
      assert(input[0].size() == o.size());
      auto tmp = computeTHDsimple(p,o,b->getSampleRate());
      tmp.raw_data.inputs=vector({in});
      res.push_back(tmp);
    }
  return res;
}


