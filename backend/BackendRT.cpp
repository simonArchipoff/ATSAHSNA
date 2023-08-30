#include "BackendRT.h"
#include <cstring>
#include "../helpers.h"




void RTModule::requestResponse(ParamResponse p){

}
void RTModule::setContinuous(bool){

}
void RTModule::setIntegrationSize(int s){

}
void RTModule::rt_init(int sampleRate){

}
void RTModule::rt_process(vector<VD> & inputs, const vector<VD> & outputs){

}
void RTModule::rt_after_process(){
    
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


void Acquisition::rt_process(VD & input, const VD & output){
    if(state == 0){
        memset(input.data(),0,sizeof(input[0])*input.size());
        return;
    }
    if(state & SENDING){
        rt_process_sending(input);
    }
    if(state & WAITRESPONSE){
        rt_process_wait_response(output);
    }
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
void Acquisition::rt_process_wait_response(const VD & output){
    uint frames=output.size();
    if(rb.freespace() < frames){
        rb.pop(frames-rb.freespace());
    }
    rb.write(output);
    time_waited += frames;
    //std::cerr << rb.available() << std::endl;
    if(rb.available() >= p.dc.getSize()){
        auto r = p.dc.getDelays(array_VD_to_VCD(rb.read(p.dc.getSize())));
        if(r.second > 0 * 0.1 * p.dc.getSize()/2){
            rb.pop(p.dc.getSize());
            //send result
            std::cerr << "result " <<  r.first + time_waited - p.dc.getSize() << std::endl;
        } else {
            if(time_waited > max_wait_response){
                //std::cerr << "something fucked up"<< std::endl;
            }
        }
    }
}

