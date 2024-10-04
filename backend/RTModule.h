#pragma once

#include "../constants.h"
#include "RingBuffer.h"
#include <Analysis.h>
#include <Harmonics.h>
#include <Response.h>
#include "../helpers.h"
#include <memory>
#include "Response.h"
#include "concurrentqueue.h"
#include <AudioIO.h>
#include "Acquisition.h"

#include "../benchmark.h"

class RTModule {
public:
    virtual void rt_process(const AudioIO<float>& inputs, AudioIO<float> & outputs) = 0;
    virtual void rt_after_process(){};
};

class RTModuleResponse : public RTModule {
public:
    RTModuleResponse(uint sampleRate, ParamResponse p, int integration_number,uint number_inputs):acq(computeChirp(p,  sampleRate),SenderMode::All,2,4800,number_inputs,10000),
        benchmark("RTModuleResponse"){
        assert(sampleRate > 0);
        this->integration_number = integration_number;
        this->sampleRate = sampleRate;
        //acq.init(s);
        //acq.start();
    }
    virtual ~RTModuleResponse(){
        benchmark.printResults();
    }

    bool tryGetResponse(ResultResponse & response){
        ReceiverResult r;
        while(acq.tryGetResult(r)){
            acc_raw_signal.add(r.signal);
            //acc_delay.add(static_cast<double>(r.time));
            // to_file("/tmp/r" + std::to_string(acc_raw_signal.size), r.signal);
            if(acc_raw_signal.size > 0){
                auto o = acc_raw_signal.get();

                VD od(o.begin(),o.end());
                VD id(acq.getSignal().begin(), acq.getSignal().end());
                response = computeResponse(paramResponse,id,od,sampleRate);
                response.name = std::to_string(r.idx);
                return true;
            }
        }
        return false;
    }

    void setContinuous(bool);
    void setIntegrationSize(int s=1);

    virtual void rt_process(const AudioIO<float> & inputs, AudioIO<float>& outputs) override {
        //benchmark.start();
        acq.rt_process(inputs,outputs);
        //benchmark.stop();
        //benchmark.printResults();
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
    Benchmark benchmark;
};



struct RTModuleHandler{
public:
    ~RTModuleHandler(){
        m_response.reset();
    }
    void setModule(std::shared_ptr<RTModule> m){
        toRTQueue.enqueue(m);
    }
    void startResponse(ParamResponse p, bool continuous, int integration){
        if(this->m_response){
            m_response.reset();
        }
        m_response = std::make_shared<RTModuleResponse>(sampleRate, p, integration,numberInputs);
        setModule(m_response);
    }
    void startHarmonics();

    bool getResultHarmonics(vector<ResultHarmonics>& result){
        result.clear();
        return false;
    }
    bool getResultResponse(vector<ResultResponse>& result){
        result.clear();

        ResultResponse r;
            if(m_response && m_response->tryGetResponse(r)){
                result.resize(1);
                result[0] = r;
                return true;
            } else {
                return false;
            }

        return false;
    }

protected:
    void setSampleRate(uint sr){
        sampleRate = sr;
    }
    void setNumberInputs(uint n){
        numberInputs = n;
    }

    void rt_process(const AudioIO<float> & inputs, AudioIO<float> & outputs){
        rt_updateModule();
        if(!rt_module){
            for(auto & v:outputs){
                std::fill(v.begin(),v.end(),0.0);
            }
        } else {
            rt_module->rt_process(inputs, outputs);
        }
    }

    void rt_after_process(){
        if(rt_module){
            rt_module->rt_after_process();
        }
    }

    moodycamel::ConcurrentQueue<std::shared_ptr<RTModule>> toRTQueue;

    std::shared_ptr<RTModule> rt_module;

private:
    void rt_updateModule(){
        std::shared_ptr<RTModule> m;
        if(toRTQueue.try_dequeue(m)){
            rt_module = m;
        }
    }
    std::shared_ptr<RTModuleResponse> m_response;
    uint sampleRate = 0;
    uint numberInputs = 0;
};
