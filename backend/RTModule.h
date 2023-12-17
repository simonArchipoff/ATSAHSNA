#pragma once

#include "../constants.h"
#include "RingBuffer.h"
#include <Analysis.h>
#include <Harmonics.h>
#include <Response.h>
#include <tuple>
#include <variant>
#include "../helpers.h"
#include <set>
#include <memory>
#include "Response.h"
#include "concurrentqueue.h"
#include <AudioIO.h>
#include "Acquisition.h"

template<typename T>
class RTModule {
public:
  virtual void rt_process(const AudioIO<T>& inputs, AudioIO<T> & outputs) = 0;
  virtual void rt_after_process(){};
};


template<typename T>
class RTModuleResponse : public RTModule<T> {
public:
  RTModuleResponse(uint sampleRate, ParamResponse p, int integration_number){
  assert(sampleRate > 0);
  this->integration_number = integration_number;
  this->sampleRate = sampleRate;
  auto s = computeChirp(p,  sampleRate);
  acq.init(s);
  acq.start();
}

  void startResponse();
  bool tryGetResponse(ResultResponse & response){
    typename Acquisition<T>::result r;
    /*while(responseQueue.try_dequeue(r)){
      acc_raw_signal.add(r.result);
      acc_delay.add(static_cast<double>(r.delay));
    }*/
    if(acc_raw_signal.size > 0){
      auto o = acc_raw_signal.get();
      //auto in = acq.getSignal();
      //response = computeResponse(paramResponse,in,o,sampleRate);
      return true;
    }
    return false;
  }

  void setContinuous(bool);
  void setIntegrationSize(int s=1);

  virtual void rt_process(const AudioIO<T> & inputs, vector<VD> & outputs) {
    auto r = acq.rt_process(inputs[0],outputs[0]);
    try{
      //responseQueue.enqueue(std::get<Acquisition<T>::result>(r));
    } catch(const std::bad_variant_access& ex){
    }

  }
  virtual void rt_after_process() override;

private:
  //    VCD chirp;
  Acquisition<T> acq;
  uint sampleRate;
  ParamResponse paramResponse;
  bool continuous;
  int integration_number;

  //accumulate results
  Accumulate<vector<T>,T> acc_raw_signal;
  Accumulate<T,T> acc_delay;
};


template<typename T>
class RTModuleHandler{
public:
  void setModule(std::shared_ptr<RTModule<T>> m){
    toRTQueue.enqueue(m);
  }
  void startResponse(ParamResponse p,bool continuous,int integration){
    if(this->m){
      m.reset();
    }
    //m = std::make_shared<RTModuleResponse<T>>(sampleRate, p, integration);
    //setModule(responseRTModule);
  }
  void startHarmonics();

  bool getResultHarmonics(vector<ResultHarmonics>& result){
    result.clear();
    abort();
    return false;
  }
  bool getResultResponse(vector<ResultResponse>& result){
    /*result.clear();
    result.resize(1);
    ResultResponse r;
    if(responseRTModule && responseRTModule->tryGetResponse(r)){
      result[0] = r;
      return true;
    } else {
      result.clear();
      return false;  
    }*/
  }

protected:
  void setSampleRate(uint sr){
    sampleRate = sr;
  }

  void rt_process(const AudioIO<float> & inputs, AudioIO<float> & outputs){
    (void)outputs;
    rt_updateModule();
    if(!module){
      for(auto & v:outputs){
	std::fill(v.begin(),v.end(),0.0);
      }
    } else {
      module->rt_process(inputs, outputs);
    }
  }

  void rt_after_process(){
    if(module){
      module->rt_after_process();
    }
  }

  moodycamel::ConcurrentQueue<std::shared_ptr<RTModule<T>>> toRTQueue;

  std::shared_ptr<RTModule<T>> module;

private:
  void rt_updateModule(){
    if(toRTQueue.try_dequeue(m)){
      module = m;
    }
  }
  std::shared_ptr<RTModule<T>> m;
  uint sampleRate = 0;
};
