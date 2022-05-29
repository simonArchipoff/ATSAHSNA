#pragma once
#include <vector>
#include <variant>
#include <optional>
#include "measure.h"
#include "concurrentqueue.h"

#include <QtConcurrent/QtConcurrent>



struct RequestMeasure{vector<VD> input;};
struct RequestPartialOutput{};
//struct RequestOutput{};
struct CancelMeasure{};
typedef std::variant<RequestMeasure,
                     RequestPartialOutput,
                     CancelMeasure> request;

struct PartialOutput{vector<VD> output;};
struct Output {vector<VD> output;};
typedef std::variant<PartialOutput,Output> response;

class Backend {
public:
  virtual uint numberInput() const = 0;
  virtual uint numberOutput() const = 0;
  virtual uint getSampleRate() const = 0;
  virtual bool isReady() const = 0;
  void requestMeasure(vector<VD> input){
    assert(input.size() == numberInput());
    requests.enqueue(RequestMeasure{input});
    notify();
  }
  void requestPartialOutput(){
    requests.enqueue(RequestPartialOutput{});
    notify();
  }
  void cancelMeasure(){
    requests.enqueue(CancelMeasure{});
    notify();
  }

  std::optional<response> tryGetResponse(){
    response r;
    if(responses.try_dequeue(r)){
        return std::optional{r};
      } else {
        return std::nullopt;
      }
  }

  std::optional<vector<VD>> tryGetOutput(){
    auto r = tryGetResponse();
    if(r.has_value()) {
        try{
          return std::optional{std::get<struct Output>(r.value()).output};
        } catch(const std::bad_variant_access& e) {
          return std::nullopt;
        }
      }
    return std::nullopt;
  }

  void setLatency(uint l){
    latency = l;
  }
  uint getLatencySample(){
    return latency;
  }

protected:
  int latency = 0;
  virtual void notify(){}
  moodycamel::ConcurrentQueue<request>  requests;
  moodycamel::ConcurrentQueue<response> responses;
};



