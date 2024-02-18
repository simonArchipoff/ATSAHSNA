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
#include <AudioIO.h>
#include "Acquisition.h"


class RTModule {
public:
    virtual void rt_process(const AudioIO<float>& inputs, AudioIO<float> & outputs) = 0;
    virtual void rt_after_process(){};
};



class RTModuleResponse : public RTModule {
public:
    RTModuleResponse(uint sampleRate, ParamResponse p, int integration_number):acq(computeChirp(p,  sampleRate),SenderMode::All,1,0,1,10000){
        assert(sampleRate > 0);
        this->integration_number = integration_number;
        this->sampleRate = sampleRate;
        //acq.init(s);
        //acq.start();
    }
    virtual ~RTModuleResponse(){}

    void startResponse();
    bool tryGetResponse(ResultResponse & response){
        ReceiverResult r;
        while(acq.tryGetResult(r)){
          acc_raw_signal.add(r.signal);
          //acc_delay.add(static_cast<double>(r.time));
        }
        if(acc_raw_signal.size > 0){
            auto o = acc_raw_signal.get();
          VD od(o.begin(),o.end());
            VD id(acq.getSignal().begin(), acq.getSignal().end());
            response = computeResponse(paramResponse,id,od,sampleRate);
            return true;
        }
        return false;
    }

    void setContinuous(bool);
    void setIntegrationSize(int s=1);

    virtual void rt_process(const AudioIO<float> & inputs, AudioIO<float>& outputs) override {
        acq.rt_process(inputs,outputs);
        ReceiverResult r;

        try{

        } catch(const std::bad_variant_access& ex){
        }

    }
    virtual void rt_after_process() override{}

private:
    //    VCD chirp;
    Acquisition acq;
    uint sampleRate;
    ParamResponse paramResponse;
    bool continuous;
    int integration_number;

    //accumulate results
    Accumulate<vector<float>,float> acc_raw_signal;
    Accumulate<float,float> acc_delay;
};



struct RTModuleHandler{
public:
    void setModule(std::shared_ptr<RTModule> m){
        toRTQueue.enqueue(m);
    }
    void startResponse(ParamResponse p,bool continuous,int integration){
        if(this->m){
            m.reset();
        }
        m = std::make_shared<RTModuleResponse>(sampleRate, p, integration);
        setModule(m);
    }
    void startHarmonics();

    bool getResultHarmonics(vector<ResultHarmonics>& result){
        result.clear();
        abort();
        return false;
    }
    bool getResultResponse(vector<ResultResponse>& result){
        /*result.clear();
        result.resize(1);
        ResultResponse r;
        if(responseRTModule && responseRTModule->tryGetResponse(r)){
          result[0] = r;
          return true;
        } else {
          result.clear();
          return false;
        }*/
        return false;
    }

protected:
    void setSampleRate(uint sr){
        sampleRate = sr;
    }

    void rt_process(const AudioIO<float> & inputs, AudioIO<float> & outputs){
        (void)outputs;
        rt_updateModule();
        if(!module){
            for(auto & v:outputs){
                std::fill(v.begin(),v.end(),0.0);
            }
        } else {
            module->rt_process(inputs, outputs);
        }
    }

    void rt_after_process(){
        if(module){
            module->rt_after_process();
        }
    }

    moodycamel::ConcurrentQueue<std::shared_ptr<RTModule>> toRTQueue;

    std::shared_ptr<RTModule> module;

private:
    void rt_updateModule(){
        if(toRTQueue.try_dequeue(m)){
            module = m;
        }
    }
    std::shared_ptr<RTModule> m;
    uint sampleRate = 0;
};
