#include "RTModule.h"
#include <cstring>
#include <memory>
#include "../helpers.h"
#include "Analysis.h"
#include <iostream>
#include <variant>
#include <algorithm>



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

void Sender::rt_output(int time, float **output, int nb_output, int nb_frames){
    assert(nb_output>0);
    assert(nb_frames>0);
    float * out = output[current_output];
    int idx = 0;
    while(idx < nb_frames){
        switch(state){
        case Sending:
            idx = rt_send(idx,out, nb_frames);
            assert(idx <= nb_frames);
            break;
        case SendingFinished:
            if(mode == RoundRobin){
                current_output = current_output + 1 % nb_output;
                out = output[current_output];

            }
            if((current_output == 0 || mode == All) && ++current_number_rec == number_rec){
                state = Finished;
            } else {
                state = Timeoffing;
                current_timeoff=0;
            }
            break;

        case Timeoffing:
            idx = rt_timeoff(idx,out,nb_frames);
            break;
        case TimeoffFinished:
            state = Sending;
            current_send = 0;
            break;
        case Finished:
            return;
            break;
        }
    }

    if(mode == All){
        for(int i = 0; i < nb_output; i++){
            if(output[i] != out)
                std::copy(out,out + nb_frames, output[i]);
        }
    }
}

int Sender::rt_timeoff(int start_idx, float * output, int nb_frames){
    assert(state == Timeoffing);
    assert(start_idx < nb_frames);
    assert(current_timeoff < timeoff);
    (void)output;
    const auto current_timeoff_left = timeoff - current_timeoff;
    if( current_timeoff_left > nb_frames - start_idx){
        current_timeoff = nb_frames - start_idx;
        return nb_frames;
    } else {
        state = TimeoffFinished;
        return start_idx + current_timeoff_left;
    }
}
int Sender::rt_send(int start_idx, float * output, int nb_frames){
    assert(state == Sending);
    assert(current_number_rec < number_rec);
    const int signalsize = static_cast<int>(signal.size());
    assert(start_idx < nb_frames);
    assert(current_send < signalsize);
    const auto current_sending_left = signalsize - current_send;
    const auto left = nb_frames - start_idx;
    if(current_sending_left <= left){
        std::transform(signal.begin() + current_send
                       ,signal.end()
                       ,output + start_idx
                       ,[](double s){return static_cast<float>(s);});

        state = SendingFinished;
        return start_idx + left;
    } else {// currend_sending_left > left
        std::transform(signal.begin() + current_send
                       ,signal.begin() + current_send + left
                       ,output + start_idx
                       ,[](double s){return static_cast<float>(s);});
        current_send += left;

        assert(start_idx + left == nb_frames);
        return start_idx + left;
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
    rb.reset(3*size);
}


Acquisition::ret_type Acquisition::rt_process(const VD & input, VD & output){
    if(state == DISABLED){
        memset(output.data(),0,sizeof(output[0])*output.size());
        return ret_type(std::monostate());
    }
    if(state & SEND){
        rt_process_sending(output);
    } else {
        memset(output.data(),0,sizeof(output[0])*output.size());
    }
    if(state & RECIEVE){
        return rt_process_wait_response(input);
    }

    abort();
}


void Acquisition::rt_process_sending(VD &output){
    assert(state & SEND);
    uint remSize = p.signal.size() - sending_index;
    uint frames = output.size();

    memcpy(output.data(), p.signal.data()+sending_index, sizeof(output[0]) * std::min<uint>(remSize,frames));
    if(remSize <= frames){
        memset(output.data()+remSize, 0, frames-remSize);
        state &= ~SEND;
        sending_index = 0;
    }else{
        sending_index+=frames;
    }
}

Acquisition::ret_type Acquisition::rt_process_wait_response(const VD & input){

    uint frames=input.size();
    if(rb.freespace() < frames){
        rb.pop(frames-rb.freespace());
    }
    result res;
    rb.write(input);
    time_waited += frames;

    if(rb.available() >= p.signal.size()*2){
        auto tab = rb.read(p.signal.size()*2);
        auto r = p.dc.getDelays(array_VD_to_VCD(tab));
        //auto f = compute_delay_fft(tab,p.signal);
        //send result
        res.level = r.second;

        if(res.level > p.threshold_level){
            res.result.resize(p.signal.size());
            std::copy(tab.begin() + r.first, tab.begin() + r.first + p.signal.size(),res.result.begin());
            res.delay = r.first + time_waited - rb.available(); //r.first + time_waited - p.dc.getSize();
            rb.pop(p.signal.size()+r.first);
            //state &= ~RECIEVE;
            return ret_type(res);
        } else {
            if(time_waited > p.timeout){
                state = DISABLED;
                std::cerr << "timeout\n";
                return ret_type(timeout());
            }
            rb.pop(input.size());
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
        acc_raw_signal.add(r.result);
        acc_delay.add(static_cast<double>(r.delay));
    }
    if(acc_raw_signal.size > 0){
        auto o = acc_raw_signal.get();
        auto in = acq.getSignal();
        response = computeResponse(paramResponse,in,o,sampleRate);
        return true;
    }

    return false;
}




