#pragma once

#include "backend.h"
#include <jack/jack.h>
#include "RTModule.h"
#include "qtmetamacros.h"

#include <mutex>
#include <variant>
#include <optional>

#include <iostream>

#include <QtCore>
#include <QSet>


using std::vector;
using std::string;



struct ParamJack {
    int numberInput;
    int numberOutput;
    vector<string> connectInputs;
    vector<string> connectOutputs;
    //vector<string> outputNames;
};


class BackendJack : public Backend, protected RTModuleHandler
{
public:
    BackendJack();
    virtual ~BackendJack();

    void start();
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


protected:

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
        std::cerr << code << reason << std::endl;
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
        std::cerr << "new buffer size " << nframes << std::endl;
        return 0;
    }

    static int jackBufferSizeCallback(jack_nframes_t nframes, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        return backend->jack_buffer_size(nframes);
    }

    virtual int jack_samplerate(jack_nframes_t nframes){
        std::cerr << "new sample rate " << nframes << std::endl;
        return 0;
    }
    static int jackSampleRateCallback(jack_nframes_t nframes, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        return backend->jack_samplerate(nframes);
    }


    virtual void jack_client_registration (const char *name, int i){
        std::cerr << "client registration " << name << " " << i  << std::endl;
    }
    static void jackClientRegistrationCallback (const char *name, int i, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);

        return backend->jack_client_registration(name,i);
    }

    virtual void jack_port_registration(jack_port_id_t port, int i,std::string port_name){
        std::cerr << "new port " << port << " " << i  << " " <<  port_name << std::endl ;
    }

    static void jackPortRegistrationCallback(jack_port_id_t port, int i, void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        assert(backend->client);
        auto p = jack_port_by_id(backend->client,port);
        assert(p);
        std::string port_name(jack_port_name(p));
        backend->jack_port_registration(port,i,port_name);
    }

    virtual void jack_port_rename(jack_port_id_t port, const char *old_name, const char *new_name){
        std::cerr << "port rename " << port << " " << old_name << new_name  << std::endl;
    }

    static void jackPortRenameCallback(jack_port_id_t port, const char *old_name, const char *new_name,void * b){
        BackendJack * backend = static_cast<BackendJack *>(b);
        backend->jack_port_rename(port,old_name,new_name);
    }

    virtual void jack_port_connect(jack_port_id_t a, jack_port_id_t b, int connect){
        auto na = jack_port_name(jack_port_by_id(client,a));
        auto nb = jack_port_name(jack_port_by_id(client,b));
        std::cerr << na << " " <<  nb  << " " << connect << std::endl;
    }

    //int 	jack_set_port_connect_callback (jack_client_t *, JackPortConnectCallback connect_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    static void jackPortConnectCallback(jack_port_id_t a, jack_port_id_t b, int connect, void * back){
        BackendJack * backend = static_cast<BackendJack *>(back);
        backend->jack_port_connect(a,b,connect);
    }


    virtual int jack_xrun(){
        std::cerr << "xrun" << std::endl ;
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
    float outputGain = 0;
};


class QJack :public QObject, public BackendJack{
    Q_OBJECT
signals:
    void jack_shutdown_s();
    void jack_info_shutdown_s(jack_status_t code, const char *reason);
    int jack_buffer_size_s(jack_nframes_t nframes);
    int jack_samplerate_s(jack_nframes_t nframes);
    void jack_client_registration_s(QString name, int i);
    void jack_port_registration_s(jack_port_id_t port, int i,QString name);
    void jack_port_rename_s(jack_port_id_t port,  QString old_name, QString new_name);
    void jack_port_connect_s(jack_port_id_t a, jack_port_id_t b, int connect, QString porta, QString portb);
    //int 	jack_set_port_connect_callback (jack_client_t *, JackPortConnectCallback connect_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    void jack_xrun_s();

protected:
    virtual void jack_shutdown(){
        ready = false;
    }
    virtual void jack_info_shutdown(jack_status_t code, const char *reason){
        BackendJack::jack_info_shutdown(code,reason);
    }
    virtual int jack_buffer_size(jack_nframes_t nframes){
        emit jack_buffer_size_s(nframes);
        return BackendJack::jack_buffer_size(nframes);
    }
    virtual int jack_samplerate(jack_nframes_t nframes){
        emit jack_samplerate_s(nframes);
        return BackendJack::jack_samplerate(nframes);
    }
    virtual void jack_client_registration (const char *name, int i){
        emit jack_client_registration_s(QString(name),i);
        BackendJack::jack_client_registration(name,i);
    }
    virtual void jack_port_registration(jack_port_id_t port, int i,std::string port_name){
        jack_port_t *p = jack_port_by_id(client, port);
        emit jack_port_registration_s(port,i,QString(jack_port_name(p)));
        BackendJack::jack_port_registration(port,i,port_name);
    }
    virtual void jack_port_rename(jack_port_id_t port, const char *old_name, const char *new_name){
        BackendJack::jack_port_rename(port,old_name,new_name);
    }
    virtual void jack_port_connect(jack_port_id_t a, jack_port_id_t b, int connect){
        auto na = jack_port_name(jack_port_by_id(client,a));
        auto nb = jack_port_name(jack_port_by_id(client,b));
        emit jack_port_connect_s(a,b,connect, QString(na),QString(nb));
        BackendJack::jack_port_connect(a,b,connect);
    }
    //int 	jack_set_port_connect_callback (jack_client_t *, JackPortConnectCallback connect_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    virtual int jack_xrun(){
        BackendJack::jack_xrun();
        return 0;
    }


};
