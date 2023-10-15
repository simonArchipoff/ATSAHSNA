#include "RTModule.h"
#include <cstring>
#include <memory>
#include "../helpers.h"
#include <iostream>
#include <variant>



void RTModuleHandler::rt_process(const vector<VD> & inputs, vector<VD> & outputs){
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

void RTModuleHandler::setSampleRate(uint sr){
    sampleRate = sr;
}

void RTModuleHandler::rt_after_process(){
    if(module){
        module->rt_after_process();
    }
}


void RTModuleHandler::rt_updateModule(){
    std::shared_ptr<RTModule> m;
    if(toRTQueue.try_dequeue(m)){
        module = m;
    }
}

void RTModuleHandler::setModule(std::shared_ptr<RTModule> m){
    toRTQueue.enqueue(m);
}



void RTModuleHandler::startResponse(ParamResponse p, bool continuous, int integration){
    if(this->responseRTModule){
        responseRTModule.reset();
    }
    responseRTModule = std::make_shared<RTModuleResponse>(sampleRate, p, integration);
    setModule(responseRTModule);
}
void RTModuleHandler::startHarmonics(){

}

bool RTModuleHandler::getResultHarmonics(vector<ResultHarmonics>& result){
    result.clear();
    abort();
    return false;
}
bool RTModuleHandler::getResultResponse(vector<ResultResponse>& result){
    result.clear();
    result.resize(1);
    ResultResponse r;
    if(responseRTModule && responseRTModule->tryGetResponse(r)){
        result[0] = r;
        return true;
    } else {
        result.clear();
        return false;  
    }

}




void Acquisition::init(const VCD & s, double threshold){
    uint size = s.size();
    p.signal.resize(size);
    p.threshold_level=threshold;
    std::transform(s.begin()
                   ,s.end()
                   ,p.signal.begin(),[](std::complex<double> c){return c.real();});
    p.dc.setReference(s);
    time_waited = 0;
    rb.reset(2*size);
}


Acquisition::ret_type Acquisition::rt_process(const VD & input, VD & output){
    if(state == DISABLED){
        memset(output.data(),0,sizeof(output[0])*output.size());
        return ret_type(std::monostate());
    }
    if(state & SEND){
        rt_process_sending(output);
    }
    if(state & RECIEVE){
        return rt_process_wait_response(input);
    }

    abort();
}


void Acquisition::rt_process_sending(VD &input){
    assert(state & SEND);
    uint remSize = p.signal.size() - sending_index;
    uint frames = input.size();

    memcpy(input.data(), p.signal.data()+sending_index, sizeof(input[0]) * std::min<uint>(remSize,frames));
    if(remSize <= frames){
        memset(input.data()+remSize, 0, frames-remSize);
        state &= ~SEND;
        sending_index = 0;
    }else{
        sending_index+=frames;
    }
}

Acquisition::ret_type Acquisition::rt_process_wait_response(const VD & output){

    uint frames=output.size();
    if(rb.freespace() < frames){
        rb.pop(frames-rb.freespace());
    }
    result res;
    rb.write(output);
    time_waited += frames;

    if(rb.available() >= p.dc.getSize()){
        auto tab = rb.read(p.dc.getSize());
        auto r = p.dc.getDelays(array_VD_to_VCD(tab));
        //send result
        res.level = r.second;

        if(res.level > p.threshold_level){
            res.result.resize(p.signal.size());
            std::copy(tab.begin() + r.first, tab.begin() + r.first + p.signal.size(),res.result.begin());
            res.delay = r.first + time_waited - rb.available(); //r.first + time_waited - p.dc.getSize();
            //rb.pop(p.signal.size()+r.first);
            state &= ~RECIEVE;
            std::cerr << "recieved\n";
            return ret_type(res);
        } else {
            if(time_waited > p.timeout){
                state = DISABLED;
                            std::cerr << "timeout\n";
                return ret_type(timeout());
            }
            rb.pop(output.size());
        }
    }
    return ret_type(std::monostate());
}




RTModuleResponse::RTModuleResponse(uint sampleRate, ParamResponse p, int integration_number){
    assert(sampleRate > 0);
    this->integration_number = integration_number;
    this->sampleRate = sampleRate;
    auto s = computeChirp(p,  sampleRate);
    acq.init(s);
    acq.start();
}


void RTModuleResponse::rt_process(const vector<VD> & inputs, vector<VD> & outputs){
    auto r = acq.rt_process(inputs[0],outputs[0]);
    try{
        responseQueue.enqueue(std::get<Acquisition::result>(r));
    } catch(const std::bad_variant_access& ex){
    }

}
//std::transform(res.begin(),res.end(),v.begin(),res.begin(),std::plus<double>());
void RTModuleResponse::rt_after_process(){

}

bool RTModuleResponse::tryGetResponse(ResultResponse & response){
    Acquisition::result r;
    while(responseQueue.try_dequeue(r)){
        acc.add(r.result);
    }
    if(acc.size > 0){
        auto o = acc.get();
        auto in = array_VCD_to_VD(chirp);
        response = computeResponse(paramResponse,in,o,sampleRate);
        return true;
    }

    return false;
}




