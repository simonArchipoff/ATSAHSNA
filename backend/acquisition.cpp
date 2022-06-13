#include "acquisition.h"
#include "backend/spectrogram.h"
#include <QDebug>
#include <algorithm>

void pad_right_0(uint n, vector<VD> & in){
  for(auto &i : in){
      i.resize(i.size()+n,0.0);
    }
}

void remove_left(uint n, vector<VD> & in){
  for(auto &i : in){
      std::rotate(i.begin(),i.begin()+n,i.end());
      i.resize(i.size()-n);
    }
}


vector<VD> acquire_output(Backend *b,const vector<VD> &input){
  b->lock.lock();
  auto in = vector{input};
  auto l = b->getLatencySample();
  pad_right_0(l,in);

  b->requestMeasure(in);
  do{
      QThread::msleep(20);
      auto r  = b->tryGetOutput();
      if(r.has_value()){
          auto out = r.value();
          remove_left(l,out);
          b->lock.unlock();
          return out;
        }
    }while(true);
}


vector<struct ResultResponse> compute_response(Backend *b, const struct ParamResponse p){
  const auto in = impulse(p.freqMin,b->getSampleRate());
  vector<VD> input;
  for(uint i = 0; i<b->numberInput(); i++){
    input.push_back(in);
    }
  auto output = acquire_output(b,input);
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
  auto output = acquire_output(b,input);
  vector<struct ResultSpectrogram> res;
  for(auto &o : output){
      res.push_back(spectrogram(o,p.nb_octave,p.resolution,b->getSampleRate()));
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
  auto output = acquire_output(b,input);

  vector<struct ResultTHD> res;
  for(auto &o : output){
      assert(input[0].size() == o.size());
      auto tmp = computeTHDNsimple(p,o,b->getSampleRate());
      tmp.raw_data.inputs=vector({in});
      res.push_back(tmp);
    }
  return res;
}


