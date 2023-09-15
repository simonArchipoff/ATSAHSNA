#pragma once

#include "../constants.h"
#include "backendRingBuffer.h"
#include <signalAnalysis.h>
#include <signalHarmonics.h>
#include <signalResponse.h>
#include <tuple>
#include <variant>
#include "../helpers.h"
#include <set>

#include "concurrentqueue.h"

class RTModule {
public:
    virtual void rt_process(vector<VD> & inputs, const vector<VD> & outputs) = 0;
    virtual void rt_after_process(){};
};



class RTModuleHandler{

    RTModule * setModule(RTModule *);

protected:
    void rt_process(vector<VD> & inputs, const vector<VD> & outputs);
    void rt_after_process();


    moodycamel::ConcurrentQueue<RTModule *> toRTQueue,fromRTQueue;

    RTModule * module;

private:
    void rt_updateModule();
};


struct SystemDescriptor{
    std::set<int> inputs;
    std::set<int> outputs;
};



class Acquisition{
public:
    Acquisition():state(DISABLED){
    }

    struct no_result{
        int dummy;
    };
    struct timeout{
        int dummy;
    };

    struct result {
        double level = 0.0;
        VD result;
        int delay;
    };

    typedef std::variant<result,no_result,timeout> ret_type;

    void init(const VCD & signal,double threshold=0.9);

    void start(){
        state = ENABLED;
        time_waited = 0;
        sending_index = 0;
        rb.reset();
    }

    ret_type rt_process(VD & input, const VD & output);


protected:
    struct params {
        VD signal;
        DelayComputer dc;
        double threshold_level;
        uint timeout;
        int sample_between_iteration;
    };


    enum state_t {
        DISABLED = 0
        ,ENABLED = 2
    };

    volatile uint state;
    uint sending_index;

    void rt_process_sending(VD & input);
    uint time_waited;
    Acquisition::ret_type rt_process_wait_response(const VD & output);

    RingBuffer<double> rb;
    params p;
};



class RTModuleResponse : public RTModule {
public:
    RTModuleResponse(uint sampleRate, ParamResponse p, int integration_number = 1);

    void startResponse();
    bool tryGetResponse(ResultResponse & v);

    void setContinuous(bool);
    void setIntegrationSize(int s=1);


    virtual void rt_process(vector<VD> & inputs, const vector<VD> & outputs) override ;
    virtual void rt_after_process() override;

private:
    VCD chirp;
    Acquisition acq;
    uint sampleRate;
    ParamResponse paramResponse;
    bool continuous;
    int integration_number;
    moodycamel::ConcurrentQueue<Acquisition::result> responseQueue;

    //accumulate results
    Accumulate<VD,double> acc;
};
