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
    uint getBufferSize() const {
      return jack_get_buffer_size(client);
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

signals:
    void shutdown();
    void sample_rate_change();
    void buffer_size_change();
    void client_registration();
    void port_registration();
    void port_rename();
    void port_connect();

public:
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

/*
int 	jack_set_thread_init_callback (jack_client_t *client, JackThreadInitCallback thread_init_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
void 	jack_on_shutdown (jack_client_t *client, JackShutdownCallback function, void *arg) JACK_OPTIONAL_WEAK_EXPORT
void 	jack_on_info_shutdown (jack_client_t *client, JackInfoShutdownCallback function, void *arg) JACK_WEAK_EXPORT
int 	jack_set_process_callback (jack_client_t *client, JackProcessCallback process_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_freewheel_callback (jack_client_t *client, JackFreewheelCallback freewheel_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_buffer_size_callback (jack_client_t *client, JackBufferSizeCallback bufsize_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_sample_rate_callback (jack_client_t *client, JackSampleRateCallback srate_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_client_registration_callback (jack_client_t *, JackClientRegistrationCallback registration_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_port_registration_callback (jack_client_t *, JackPortRegistrationCallback registration_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_port_rename_callback (jack_client_t *, JackPortRenameCallback rename_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_port_connect_callback (jack_client_t *, JackPortConnectCallback connect_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_graph_order_callback (jack_client_t *, JackGraphOrderCallback graph_callback, void *) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_xrun_callback (jack_client_t *, JackXRunCallback xrun_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
int 	jack_set_latency_callback (jack_client_t *, JackLatencyCallback latency_callback, void *) JACK_WEAK_EXPORT


typedef enum JackLatencyCallbackMode 	jack_latency_callback_mode_t
typedef void(* 	JackLatencyCallback) (jack_latency_callback_mode_t mode, void *arg)
typedef struct _jack_latency_range 	jack_latency_range_t
typedef int(* 	JackProcessCallback) (jack_nframes_t nframes, void *arg)
typedef void(* 	JackThreadInitCallback) (void *arg)
typedef int(* 	JackGraphOrderCallback) (void *arg)
typedef int(* 	JackXRunCallback) (void *arg)
typedef int(* 	JackBufferSizeCallback) (jack_nframes_t nframes, void *arg)
typedef int(* 	JackSampleRateCallback) (jack_nframes_t nframes, void *arg)
typedef void(* 	JackPortRegistrationCallback) (jack_port_id_t port, int, void *arg)
typedef void(* 	JackPortRenameCallback) (jack_port_id_t port, const char *old_name, const char *new_name, void *arg)
typedef void(* 	JackClientRegistrationCallback) (const char *name, int, void *arg)
typedef void(* 	JackPortConnectCallback) (jack_port_id_t a, jack_port_id_t b, int connect, void *arg)
typedef void(* 	JackFreewheelCallback) (int starting, void *arg)
typedef void *(* 	JackThreadCallback) (void *arg)
typedef void(* 	JackShutdownCallback) (void *arg)
typedef void(* 	JackInfoShutdownCallback) (jack_status_t code, const char *reason, void *arg)
*/
