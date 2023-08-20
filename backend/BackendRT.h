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
