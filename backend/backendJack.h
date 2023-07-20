#pragma once

#include "backend.h"
#include <jack/jack.h>
#include "concurrentqueue.h"
#include <mutex>
#include <variant>
#include <optional>

#if __has_include("QDebug")
#include <QDebug>
#else
#ifndef NDEBUG
#define QDebug() std::cerr
#else
#define QDebug() (void)
#endif //NDEBUG
#endif //has_include

using std::vector;
using std::string;



struct ParamJack {
    int numberInput;
    int numberOutput;
    vector<string> connectInputs;
    vector<string> connectOutputs;
    //vector<string> outputNames;
};




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

    bool addInputPort(std::string name="input", std::string connect_port="");
    bool addOutputPort(std::string name="output", std::string connect_port="");

    void setOutputGain(double g){
        outputGain = g;
    }
    double getOutputGain(){
        return outputGain;
    }

    virtual void setLatency(uint l){
        latency = l;
    }
    uint getLatencySample(){
        return latency;
    }

    void setLatencyAutomatic(bool b=true){
        latency_automatic = b;
    }

    vector<VD> acquisition(const vector<VD> &input);



protected:
    int latency = 0;
    bool latency_automatic = true;

    //moodycamel::ConcurrentQueue<request>  requests;
   // moodycamel::ConcurrentQueue<response> responses;
    std::mutex lock;
    static void * audio_thread(void*);

    //int 	jack_set_thread_init_callback (jack_client_t *client, JackThreadInitCallback thread_init_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    //static void jackThreadInitCallback(void *arg);



    virtual void jack_shutdown(){
        ready = false;
    }

    static void jackShutdownCallback(void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        backend->jack_shutdown();
    }


    virtual void jack_info_shutdown(jack_status_t code, const char *reason){
        qDebug() << code << reason;
    }

    static void jackInfoShutdownCallback(jack_status_t code, const char *reason, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        backend->jack_info_shutdown(code,reason);
    }

    //int 	jack_set_process_callback (jack_client_t *client, JackProcessCallback process_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    //static int jackProcessCallback(jack_nframes_t nframes, BackendJack * backend);

    //int 	jack_set_freewheel_callback (jack_client_t *client, JackFreewheelCallback freewheel_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    //static void jackFreewheelCallback(int starting, BackendJack * backend);

    virtual int jack_buffer_size(jack_nframes_t nframes){
        qDebug() << "new buffer size " << nframes;
        return 0;
    }

    static int jackBufferSizeCallback(jack_nframes_t nframes, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        return backend->jack_buffer_size(nframes);
    }

    virtual int jack_samplerate(jack_nframes_t nframes){
        qDebug() << "new sample rate " << nframes;
        return 0;
    }
    static int jackSampleRateCallback(jack_nframes_t nframes, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        return backend->jack_samplerate(nframes);
    }


    virtual void jack_client_registration (const char *name, int i){
        qDebug() << "client registration" << name << " " << i;
    }
    static void jackClientRegistrationCallback (const char *name, int i, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        return backend->jack_client_registration(name,i);
    }

    virtual void jack_port_registration(jack_port_id_t port, int i){
        qDebug() << "new port " << port << " " << i ;
    }

    static void jackPortRegistrationCallback(jack_port_id_t port, int i, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        backend->jack_port_registration(port,i);
    }

    virtual void jack_port_rename(jack_port_id_t port, const char *old_name, const char *new_name){
        qDebug() << "port rename " << port << " " << old_name << new_name;
    }

    static void jackPortRenameCallback(jack_port_id_t port, const char *old_name, const char *new_name,void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        backend->jack_port_rename(port,old_name,new_name);
    }

    virtual void jack_port_connect(jack_port_id_t a, jack_port_id_t b, int connect){
        auto na = jack_port_name(jack_port_by_id(client,a));
        auto nb = jack_port_name(jack_port_by_id(client,b));
        qDebug() << na << " " <<  nb  << " " << connect;
    }

    //int 	jack_set_port_connect_callback (jack_client_t *, JackPortConnectCallback connect_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    static void jackPortConnectCallback(jack_port_id_t a, jack_port_id_t b, int connect, void * back){
        BackendJack * backend = static_cast<BackendJack *>(back);
        backend->jack_port_connect(a,b,connect);
    }


    virtual int jack_xrun(){
        qDebug() << "xrun" ;
        return 0;
    }
    static int jackXRunCallback(void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        return backend->jack_xrun();
    }


    //int 	jack_set_latency_callback (jack_client_t *, JackLatencyCallback latency_callback, void *) JACK_WEAK_EXPORT
    //static void jackLatencyCallback(jack_latency_callback_mode_t mode, BackendJack * backend);

    bool ready = false;
    jack_client_t * client;
    std::vector<jack_port_t *> inputPorts,outputPorts;
    enum {Waiting, Measuring} status = Waiting;
    uint idx;
    double outputGain;
    vector<vector<double>> currentInput;
    vector<vector<double>> currentOutput;
};
