#pragma once

#include "../constants.h"
#include "backendRingBuffer.h"
#include <signalAnalysis.h>
#include <signalHarmonics.h>
#include <signalResponse.h>

#include "concurrentqueue.h"


class RTModule{
    void requestResponse(ParamResponse p);
    void setContinuous(bool);
    void setIntegrationSize(int s=1);
    bool tryGetResponse(vector<ResultResponse> & v){
        return responseQueue.try_dequeue(v);
    }


protected:

    void rt_init(int sampleRate);
    void rt_process(vector<VD> & inputs, const vector<VD> & outputs);
    void rt_after_process();
    moodycamel::ConcurrentQueue<vector<ResultResponse>> responseQueue;
};




class Acquisition{
public:
    Acquisition():state(DISABLED){
    }

    struct result {
        double level;
        int idx;

        VD result_uncroped;
        int delay_result;
    };

    void init(size_t sampleRate, const VCD & signal);

    void rt_process(VD & input, const VD & output);

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
    void rt_process_wait_response(const VD & output);

    RingBuffer<double> rb;
    params p;
};

