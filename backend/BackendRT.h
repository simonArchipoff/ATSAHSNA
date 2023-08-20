#pragma once

#include "concurrentqueue.h"
#include "../constants.h"

class RTModule{
public:
    virtual void rt_process(int nframe, const vector<float *> & inputs, vector<float *> & outputs)= 0;
    virtual void rt_after_process() = 0;
};

template<class request, class response>
class RTModuleWithControl : public RTModule{
    typedef request Request;
    typedef response Response;



protected:
    virtual void rt_handleRequest(Request &r);
    virtual void handleResponse(Response &);

    void rt_recieveCommand(){
        Request r;
        if(requests.try_dequeue(&r))
            rt_handleRequest(r);
    }
    void rt_sendResponse(Response &r){
        responses.enqueue(r);
    }

    void sendCommand(Request &r){
        requests.enqueue(r);
    }
    void recieveResponse(){
        Response r;
        if(responses.try_dequeue(&r))
            handleResponse(r);
    }

    moodycamel::ConcurrentQueue<Request>  requests;
    moodycamel::ConcurrentQueue<Response> responses;
};


class HandleRTModule : public RTModule{
public:
    HandleRTModule():module(nullptr){}

    virtual void rt_process(int nframe, const vector<float *> & inputs, vector<float *> & outputs) override{
        if(module)
            module->rt_process(nframe, inputs,outputs);
    }
    virtual void rt_after_process() override{
        if(module)
            module->rt_after_process();
    }


    void rt_recieveModule(){
        RTModule *m;
        if(modulequeue.try_dequeue(m)){
            module = m;
        }
    }

    void sendModule(RTModule * m){
        modulequeue.enqueue(m);
    }


    RTModule * module;
    int sampleRate;
    moodycamel::ConcurrentQueue<RTModule *> modulequeue;
};


#include "concurrentqueue.h"
#include "backend.h"
#include "backendRingBuffer.h"
#include <signalAnalysis.h>

class ControlModule {

    struct RTModule{
        void init(int sampleRate);
        void process(const vector<VD> & inputs, vector<VD> & outputs);
        void after_process();

        void recieveCommand();
        void sendResponse();
    };

    void sendCommand();
    void recieveResponse();
};

class BackendRT : public Backend{

};



class BackendResponse
{
public:
    BackendResponse():state(DISABLED){
    }

    void init(size_t sampleRate, const VCD & signal);
    void process(size_t frame, float * input, float * output);
    void start(){
        state |= SENDING;
        state |= WAITRESPONSE;
        sending_index = 0;
    }

protected:
    enum state_t {
        DISABLED = 0
        ,WAITRESPONSE = 1
        ,SENDING = 2
    };

    uint state;
    uint sending_index;
    void process_sending(size_t frames, float * input, float * output);


    uint max_wait_response;
    uint time_waited;
    void process_wait_response(size_t frames, float * input,float * output);

    RingBuffer<double> rb;
    DelayComputer dc;
    VF signal;
};

