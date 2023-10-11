#pragma once

#include "../constants.h"
#include "RingBuffer.h"
#include <Analysis.h>
#include <Harmonics.h>
#include <Response.h>
#include <tuple>
#include <variant>
#include "../helpers.h"
#include <set>

#include <memory>

#include "Response.h"

#include "concurrentqueue.h"

class RTModule {
public:
    virtual void rt_process(vector<VD> & inputs, const vector<VD> & outputs) = 0;
    virtual void rt_after_process(){};
};


class RTModuleResponse;
class RTModuleHarmonics;

class RTModuleHandler{
public:
    void setModule(std::shared_ptr<RTModule>);
    void startResponse(ParamResponse p);
    void startHarmonics();

    bool getResultHarmonics(vector<ResultHarmonics>& result);
    bool getResultResponse(vector<ResultResponse>& result);

protected:
    void setSampleRate(uint);

    void rt_process(vector<VD> & inputs, const vector<VD> & outputs);
    void rt_after_process();

    moodycamel::ConcurrentQueue<std::shared_ptr<RTModule>> toRTQueue;

    std::shared_ptr<RTModule> module;

private:
    void rt_updateModule();
    uint sampleRate = 0;
    std::shared_ptr<RTModuleResponse> responseRTModule;
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
        state = SEND | RECIEVE;
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
        ,SEND     = 2
        ,RECIEVE  = 4
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

    virtual void rt_process(vector<VD> & inputs, const vector<VD> & outputs) override;
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

