#pragma once

#include "backend.h"
#include <jack/jack.h>

using std::vector;

class BackendJack : public Backend
{
public:
    BackendJack();
    ~BackendJack();

    uint numberInput() const override {
        return inputPorts.size();
    }
    uint numberOutput() const override {
        return outputPorts.size();
    }
    uint getSampleRate() const override {
      return jack_get_sample_rate(client);
    }
    bool isReady() const override{
      return ready;
    }

    bool addInputPort(std::string name="input");
    bool addOutputPort(std::string name="output");
protected:
    static void * audio_thread(void*);
    void treatRequest();
    void sendOutput();

    bool ready = false;
    jack_client_t * client;
    std::vector<jack_port_t *> inputPorts,outputPorts;
    enum {Waiting, Measuring} status = Waiting;
    uint idx;
    vector<vector<double>> currentInput;
    vector<vector<double>> currentOutput;
};


