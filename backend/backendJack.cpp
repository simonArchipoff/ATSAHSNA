#include "backendJack.h"

#include "../constants.h"
#include <string.h>

#include <jack/types.h>
#include <jack/jack.h>
#include <mutex>
#include <stdio.h>
#if __has_include("QThread")
#define WE_HAVE_QT
#include <QThread>
#include <QDebug>
#endif


//#include <QtConcurrent/QtConcurrent>


BackendJack::BackendJack()
{
    jack_options_t options = JackNullOption;
    jack_status_t status;
    client = jack_client_open(APPNAME, options, &status, nullptr);
    if(client == nullptr){
        fprintf(stderr, "Unable to open jack client\n");
        fprintf (stderr, "jack_client_open() failed, "
                        "status = 0x%2.0x\n", status);
        exit(1);
    } else {
        jack_set_process_thread(client,audio_thread,this);

        jack_on_shutdown(client, jackShutdownCallback, this);
        jack_on_info_shutdown (client, jackInfoShutdownCallback, this);
        jack_set_buffer_size_callback (client, jackBufferSizeCallback, this);
        jack_set_client_registration_callback (client, jackClientRegistrationCallback, this);
        jack_set_port_registration_callback (client, jackPortRegistrationCallback, this);
        jack_set_port_rename_callback (client, jackPortRenameCallback, this);
        jack_set_port_connect_callback (client, jackPortConnectCallback, this);
        jack_set_xrun_callback(client, jackXRunCallback,this);
        ready = !jack_activate(client);
        outputGain = -3;
    }
}



BackendJack::~BackendJack(){
    jack_client_close(client);
}

void * BackendJack::audio_thread(void * arg){
    BackendJack * jb = static_cast<BackendJack *>(arg);

    while(1) {
        jack_nframes_t nframes = jack_cycle_wait(jb->client);
        if(jb->status == Waiting){
            for(uint i = 0; i < jb->outputPorts.size(); i++){
                auto out= (float*) jack_port_get_buffer(jb->outputPorts[i], nframes);
                if(!out)
                    continue;
                for(uint j = 0; j < nframes ; j++){
                    out[j] = 0;
                }
            }
            jack_cycle_signal(jb->client,0);
            continue;
        }
        assert(jb->status == Measuring);

        uint size_to_copy = std::min<uint>(nframes, jb->currentInput[0].size() - jb->idx);

        for(uint i = 0; i < jb->outputPorts.size(); i++){
            auto out = static_cast<jack_default_audio_sample_t*>
                (jack_port_get_buffer(jb->outputPorts[i], nframes));
            double factor = pow(10,jb->getOutputGain()/20);
            for(uint j = 0; j < size_to_copy; j++){
                out[j] = factor * static_cast<jack_default_audio_sample_t>(jb->currentInput[i][jb->idx+j]);
            }
            for(uint j = size_to_copy; j < nframes; j++){
                out[j]=0;
            }
        }
        for(uint i = 0; i < jb->inputPorts.size(); i++) {
            auto in = static_cast<jack_default_audio_sample_t*>
                (jack_port_get_buffer(jb->inputPorts[i], nframes));
            if(!in)
                continue;
            for(uint j = 0; j < size_to_copy; j++){
                jb->currentOutput[i][jb->idx+j] = static_cast<double>(in[j]);
            }
        }
        jb->idx += size_to_copy;
        if(jb->idx >= jb->currentInput[0].size()){
            assert(jb->idx == jb->currentInput[0].size());
        }

        jack_cycle_signal(jb->client,0);
    }
}


const char ** get_list_ports(jack_client_t * client){
    return jack_get_ports(client,NULL,NULL,0);
}

bool does_port_exists(jack_client_t * client, const char * port){
    const char ** s = get_list_ports(client);
    while(*s){
        assert(strchr(*s,':') < *s + strlen(*s));
        if(strcmp(strchr(*s,':')+1,port) == 0)
            return true;
        else
            s++;
    }
    return false;
}


bool BackendJack::addInputPort(std::string name, std::string connect){
    if(client){
        std::string n;
        int i=0;
        std::string{n};
        do{
            n =name + std::to_string(i++);
        }while(does_port_exists(client,n.c_str()));

        auto inputPort = jack_port_register(client,n.c_str(),
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsInput,0);
        if(inputPort){
            inputPorts.push_back(inputPort);
            auto r = jack_connect(client, connect.c_str(), n.c_str());
            return !r;
        }
    }
    return false;
}
bool BackendJack::addOutputPort(std::string name,std::string connect){
    if(client){
        std::string n;
        int i=0;
        do{
            n = name + std::to_string(i++);
        }while(does_port_exists(client,n.c_str()));
        auto outputPort = jack_port_register(client,n.c_str(),
                                             JACK_DEFAULT_AUDIO_TYPE,
                                             JackPortIsOutput,0);
        if(outputPort){
            outputPorts.push_back(outputPort);
            auto r =  jack_connect(client, n.c_str(), connect.c_str());
            return !r;
        }
    }
    return false;
}



