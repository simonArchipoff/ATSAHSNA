#include "BackendRT.h"
#include <cstring>
#include "../helpers.h"




void RTModuleHandler::requestResponse(ParamResponse p){

}
void RTModuleHandler::setContinuous(bool){

}
void RTModuleHandler::setIntegrationSize(int s){

}

void RTModuleHandler::rt_process(vector<VD> & inputs, const vector<VD> & outputs){
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


void Acquisition::init(size_t sampleRate, const VCD & s){
    uint size = s.size();
    p.signal.resize(size);
    std::transform(s.begin()
                   ,s.end()
                   ,p.signal.begin(),[](std::complex<double> c){return c.real();});
    p.dc.setReference(s);
    rb.reset(2*size);
    max_wait_response = sampleRate + size;
}


Acquisition::result Acquisition::rt_process(VD & input, const VD & output){
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

#include <iostream>
Acquisition::result Acquisition::rt_process_wait_response(const VD & output){

    uint frames=output.size();
    if(rb.freespace() < frames){
        rb.pop(frames-rb.freespace());
    }
    result res;
    rb.write(output);
    time_waited += frames;
    //std::cerr << rb.available() << std::endl;
    if(rb.available() >= p.dc.getSize()){
        auto tab = rb.read(p.dc.getSize());
        auto r = p.dc.getDelays(array_VD_to_VCD(tab));
        if(true){
            rb.pop(p.dc.getSize());
            //send result
            res.level = r.second;
            res.idx = r.first;
            res.result_uncroped = tab;
            res.delay_result = time_waited - p.dc.getSize();
            return res;
        } else {
            if(time_waited > max_wait_response){
                //std::cerr << "something fucked up"<< std::endl;
            }
        }

    }
    return res;
} 

