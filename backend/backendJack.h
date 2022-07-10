#pragma once

#include "backend.h"
#include <jack/jack.h>
#include "concurrentqueue.h"

#include <variant>
#include <optional>

#include <QFuture>

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


using std::vector;

class BackendJack : public Backend
{
public:
    BackendJack();
    virtual ~BackendJack();

    uint numberInput() const override {
        return inputPorts.size();
    }
    uint numberOutput() const  override{
        return outputPorts.size();
    }
    uint getSampleRate() const override {
      return jack_get_sample_rate(client);
    }
    bool isReady() const override {
      return ready;
    }
    void requestMeasure(vector<VD> input){
      assert(input.size() == numberInput());
      requests.enqueue(RequestMeasure{input});
    }
    void requestPartialOutput(){
      requests.enqueue(RequestPartialOutput{});
    }
    void cancelMeasure(){
      requests.enqueue(CancelMeasure{});
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

    vector<VD> acquisition(const vector<VD> &input) override;

    QFuture<vector<VD>> acquisition_async(const vector<VD> &input);

    bool addInputPort(std::string name="input");
    bool addOutputPort(std::string name="output");
    void setOutputGain(double g){
      outputGain = g;
    }
    double getOutputGain(){
      return outputGain;
    }

    std::mutex lock;
  protected:
    int latency = 0;

    moodycamel::ConcurrentQueue<request>  requests;
    moodycamel::ConcurrentQueue<response> responses;


    static void * audio_thread(void*);
    void treatRequest();
    void sendOutput();

    bool ready = false;
    jack_client_t * client;
    std::vector<jack_port_t *> inputPorts,outputPorts;
    enum {Waiting, Measuring} status = Waiting;
    uint idx;
    double outputGain;
    vector<vector<double>> currentInput;
    vector<vector<double>> currentOutput;
};


