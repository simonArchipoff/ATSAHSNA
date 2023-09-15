#include "BackendRT.h"
#include <cstring>
#include "../helpers.h"



void RTModuleHandler::rt_process(vector<VD> & inputs, const vector<VD> & outputs){
    (void)outputs;
    rt_updateModule();
    if(!module){
        for(auto & v:inputs){
            std::fill(v.begin(),v.end(),0.0);
        }
        return;
    }

}
void RTModuleHandler::rt_after_process(){
    if(module){
        module->rt_after_process();
    }
}


void RTModuleHandler::rt_updateModule(){
    RTModule * m = nullptr;
    if(toRTQueue.try_dequeue(m)){
        fromRTQueue.enqueue(module);
        module = m;
    }
}


void Acquisition::init(size_t sampleRate, const VCD & s,double threshold){
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


Acquisition::ret_type Acquisition::rt_process(VD & input, const VD & output){
    result r={.level=0};
    if(state == 0){
        memset(input.data(),0,sizeof(input[0])*input.size());
        return r;
    }
    if(state & SENDING){
        rt_process_sending(input);
    }
    if(state & WAITRESPONSE){
        return rt_process_wait_response(output);
    }
    abort();
}


void Acquisition::rt_process_sending(VD &input){
    assert(state & SENDING);
    uint remSize = p.signal.size() - sending_index;
    uint frames = input.size();
    memcpy(input.data(), p.signal.data()+sending_index, std::min<uint>(remSize,frames));
    if(remSize <= frames){
        memset(input.data()+remSize, 0, frames-remSize);
        state &= ~SENDING;
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
            //reset or something
            rb.pop(p.signal.size()+r.first);
            return ret_type(res);
        } else {
            if(time_waited > p.timeout){
                return ret_type(timeout());
            }
            rb.pop(output.size());
        }
    }
    return ret_type(no_result());
}




RTModuleResponse::RTModuleResponse(uint sampleRate, ParamResponse p, int integration_number){
    this->integration_number = integration_number;
    this->sampleRate = sampleRate;
    auto s =  computeChirp(p,  sampleRate);
    acq.init(sampleRate,s);

}


void RTModuleResponse::rt_process(vector<VD> & inputs, const vector<VD> & outputs){
    auto r = acq.rt_process(inputs[0],outputs[0]);
    try{
        responseQueue.enqueue(std::get<Acquisition::result>(r).result);
    } catch(const std::bad_variant_access& ex){
    }

}
//std::transform(res.begin(),res.end(),v.begin(),res.begin(),std::plus<double>());
void RTModuleResponse::rt_after_process(){
}



