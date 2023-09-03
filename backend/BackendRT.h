#pragma once

#include "../constants.h"
#include "backendRingBuffer.h"
#include <signalAnalysis.h>
#include <signalHarmonics.h>
#include <signalResponse.h>
#include <tuple>


#include "concurrentqueue.h"

class RTModule {
public:
    virtual void rt_process(vector<VD> & inputs, const vector<VD> & outputs) = 0;
    virtual void rt_after_process(){};
};


class RTModuleHandler{

    void requestResponse(ParamResponse p);
    void setContinuous(bool);
    void setIntegrationSize(int s=1);
    bool tryGetResponse(vector<ResultResponse> & v){
        return responseQueue.try_dequeue(v);
    }


protected:
    void rt_process(vector<VD> & inputs, const vector<VD> & outputs);
    void rt_after_process();
    moodycamel::ConcurrentQueue<vector<ResultResponse>> responseQueue;

    moodycamel::ConcurrentQueue<RTModule *> toRTQueue,fromRTQueue;

    RTModule * module;

    RTModule * setModule(RTModule *);

private:
    void rt_updateModule();
};




class Acquisition{
public:
    Acquisition():state(DISABLED){
    }

    struct result {
        double level = 0.0;
        int idx = -1;

        VD result_uncroped;
        int delay_result;
    };

    void init(size_t sampleRate, const VCD & signal);

    result rt_process(VD & input, const VD & output);

    void start(){
        state |= SENDING;
        state |= WAITRESPONSE;
        sending_index = 0;
    }

protected:
    struct params {
        VD signal;
        DelayComputer dc;
        int recurences;
        int sample_between_iteration;
    };


    enum state_t {
        DISABLED = 0
        ,WAITRESPONSE = 1
        ,SENDING = 2
    };

    volatile uint state;
    uint sending_index;

    void rt_process_sending(VD & input);
    uint max_wait_response;
    uint time_waited;
    result rt_process_wait_response(const VD & output);

    RingBuffer<double> rb;
    params p;
};

