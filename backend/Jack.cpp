#include "Jack.h"

#include "../constants.h"
#include "Harmonics.h"
#include "RTModule.h"
#include "Response.h"

#include <string.h>

#include <jack/types.h>
#include <jack/jack.h>
#include <mutex>
#include <stdio.h>
#include <variant>



//#include <QtConcurrent/QtConcurrent>


BackendJack::BackendJack()
{
}


void BackendJack::start(){
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
        jack_set_sample_rate_callback(client, jackSampleRateCallback,this);
        jack_set_client_registration_callback (client, jackClientRegistrationCallback, this);
        jack_set_port_registration_callback (client, jackPortRegistrationCallback, this);
        jack_set_port_rename_callback (client, jackPortRenameCallback, this);
        jack_set_port_connect_callback (client, jackPortConnectCallback, this);
        jack_set_xrun_callback(client, jackXRunCallback,this);
        outputGain = -3;
    }
    ready = !jack_activate(client);
    RTModuleHandler::setSampleRate(getSampleRate());
}

BackendJack::~BackendJack(){
    jack_client_close(client);
}

void * BackendJack::audio_thread(void * arg){
    BackendJack * jb = static_cast<BackendJack *>(arg);

    while(1) {
        jack_nframes_t nframes = jack_cycle_wait(jb->client);

        AudioIO<float> inputs;
        for(uint i = 0; i < jb->inputPorts.size(); i++) {
            auto in = (float*)jack_port_get_buffer(jb->inputPorts[i], nframes);
            inputs.addChannel(nframes,in);
        }

        AudioIO<float> outputs;
        int nbOutput = jb->outputPorts.size();
        for(uint i = 0; i < nbOutput; i++){
            auto out = (float*)jack_port_get_buffer(jb->outputPorts[i], nframes);
            outputs.addChannel(nframes,out);
        }

        jb->rt_process(inputs, outputs);

        jack_cycle_signal(jb->client,0);
        jb->rt_after_process();
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



BackendJack::ResultHarmonicsVar BackendJack::getResultHarmonics(){
    vector<ResultHarmonics> r;
    if(RTModuleHandler::getResultHarmonics(r)){
        return ResultHarmonicsVar(r);
    } else {
        return ResultHarmonicsVar(std::monostate());
    }
}
BackendJack::ResultResponseVar BackendJack::getResultResponse(){
    vector<ResultResponse> r;
    if(RTModuleHandler::getResultResponse(r)){
        return ResultResponseVar(r);
    } else {
        return ResultResponseVar(std::monostate());
    }
}
