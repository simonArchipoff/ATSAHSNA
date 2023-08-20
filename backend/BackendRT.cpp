#include "BackendRT.h"
#include <cstring>
#include "../helpers.h"
void BackendResponse::init(size_t sampleRate, const VCD & s){
    uint size = s.size();
    signal.resize(size);
    std::transform(s.begin()
                   ,s.end()
                   ,signal.begin(),[](std::complex<double> c){return float(c.real());});
    dc.setReference(s);
    rb.reset(2*size);
    uint max_wait_response = sampleRate + size;
}



void BackendResponse::process(size_t frames, float * input, float * output){
    if(state == 0){
        memset(input,0,sizeof(*input)*frames);
        return;
    }
    if(state & SENDING){
        process_sending(frames, input, output);
    }
    if(state & WAITRESPONSE){
        process_wait_response(frames,input,output);
    }
}


void BackendResponse::process_sending(size_t frames, float*input, float*output){
    assert(state & SENDING);
    uint remSize = signal.size() - sending_index;
    memcpy(input, signal.data()+sending_index, std::min<uint>(remSize,frames));
    if(remSize <= frames){
        memset(input+remSize, 0, frames-remSize);
        state &= ~SENDING;
        sending_index = 0;
    }else{
        sending_index+=frames;
    }
}

#include <iostream>
void BackendResponse::process_wait_response(size_t frames, float * input, float * output){
    if(rb.freespace() < frames){
        rb.pop(frames-rb.freespace());
    }
    rb.write(VD(output, output+frames));
    time_waited += frames;
    //std::cerr << rb.available() << std::endl;
    if(rb.available() >= dc.getSize()){
        auto r = dc.getDelays(array_VD_to_VCD(rb.read(dc.getSize())));
        if(r.second > 0 * 0.1 * dc.getSize()/2){
            rb.pop(dc.getSize());
            std::cerr << "result " <<  r.first + time_waited - dc.getSize() << std::endl;
        } else {
            if(time_waited > max_wait_response){
                //std::cerr << "something fucked up"<< std::endl;
            }
        }
    }
}

