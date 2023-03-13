#include "backendJack.h"

#include "../constants.h"

#include <jack/types.h>
#include <jack/jack.h>
#include <mutex>
#include <stdio.h>

#include <chrono>
#include <thread>


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
      jb->treatRequest();
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
          jb->sendOutput();
        }

      jack_cycle_signal(jb->client,0);
    }
}

bool BackendJack::addInputPort(std::string name){
  if(client){
      auto inputPort = jack_port_register(client,name.data(),
                                          JACK_DEFAULT_AUDIO_TYPE,
                                          JackPortIsInput,0);
      if(inputPort){
          inputPorts.push_back(inputPort);
          return true;
        }
    }
  return false;
}
bool BackendJack::addOutputPort(std::string name){
  if(client){
      auto outputPort = jack_port_register(client,name.data(),
                                           JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsOutput,0);
      if(outputPort){
          outputPorts.push_back(outputPort);
          return true;
        }
    }
  return false;
}


void BackendJack::treatRequest(){
  request r;
  bool b = requests.try_dequeue(r);
  struct V {
    V(BackendJack * b):t(b){};
    void operator()(RequestMeasure &r){
      //assert(t->status == Waiting);

      t->idx=0;
      t->status = Measuring;
      t->currentInput = r.input;
      t->currentOutput.resize(t->numberOutput());
      assert(t->currentInput.size() == t->inputPorts.size());
      if(t->numberInput() == 0 || t->numberOutput() == 0){
          t->sendOutput();
        }
#ifndef NDEBUG
      for(auto &i : t->currentInput){
          assert(i.size() == t->currentInput[0].size());
        }
#endif
      for(auto &i : t->currentOutput){
          i.resize(t->currentInput[0].size());
        }
    };
    void operator()(RequestPartialOutput &){
      auto r = t->currentOutput;
      for(auto &i : r){
          i.resize(t->idx);
        }
      t->responses.enqueue(PartialOutput{r});
    };
    void operator()(CancelMeasure &){
      t->status = Waiting;
      for(auto &i : t->currentInput){
          i.resize(0);
        }
      for(auto &i : t->currentOutput){
          i.resize(0);
        }
    };
    BackendJack *t;
  };
  if(b){
      std::visit(V(this),r);
    }
}

void BackendJack::sendOutput(){
  responses.enqueue(Output{currentOutput});
  status = Waiting;
}




vector<VD> BackendJack::acquisition(const vector<VD> &input){
  if(input.size()==0)
    return input;
  lock.lock();
  auto in = vector{input};
  requestMeasure(in);
  do{
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      //QThread::msleep(100);
      auto r  = tryGetOutput();
      if(r.has_value()){
          auto out = r.value();
          lock.unlock();
          return out;
        }
    }while(true);
}
