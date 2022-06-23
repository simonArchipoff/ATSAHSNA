#include "backendJack.h"
#include <jack/types.h>
#include <stdio.h>
#include <algorithm>
#include "../constants.h"
#include <QThread>

const char **ports;
const char *client_name = "simple";
const char *server_name = NULL;
jack_options_t options = JackNullOption;
jack_status_t status;

/* open a client connection to the JACK server */

/*
client = jack_client_open (client_name, options, &status, server_name);
if (client == NULL) {

	if (status & JackServerFailed) {
		fprintf (stderr, "Unable to connect to JACK server\n");
	}
	exit (1);
}
if (status & JackServerStarted) {
	fprintf (stderr, "JACK server started\n");
}
if (status & JackNameNotUnique) {
	client_name = jack_get_client_name(client);
	fprintf (stderr, "unique name `%s' assigned\n", client_name);
}
*/

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
      ready = !jack_activate(client);
    }
}

BackendJack::~BackendJack()
{
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
          for(uint j = 0; j < size_to_copy; j++){
              out[j] = static_cast<jack_default_audio_sample_t>(jb->currentInput[i][jb->idx+j]);
            }
          for(uint j = size_to_copy; j < nframes; j++){
              out[j]=0;
            }
        }
      for(uint i = 0; i < jb->inputPorts.size(); i++) {
          auto in = static_cast<jack_default_audio_sample_t*>
              (jack_port_get_buffer(jb->inputPorts[i], nframes));
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
  //jack_nframes_t 	jack_cycle_wait (jack_client_t *client) JACK_OPTIONAL_WEAK_EXPORT

  //void 	jack_cycle_signal (jack_client_t *client, int status) JACK_OPTIONAL_WEAK_EXPORT
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

void pad_right_0(uint n, vector<VD> & in){
  for(auto &i : in){
      i.resize(i.size()+n,0.0);
    }
}

void remove_left(uint n, vector<VD> & in){
  for(auto &i : in){
      std::rotate(i.begin(),i.begin()+n,i.end());
      i.resize(i.size()-n);
    }
}

vector<VD> acquire_output(BackendJack *b,const vector<VD> &input){
  b->lock.lock();
  auto in = vector{input};
  auto l = b->getLatencySample();
  pad_right_0(l,in);

  b->requestMeasure(in);
  do{
      QThread::msleep(20);
      auto r  = b->tryGetOutput();
      if(r.has_value()){
          auto out = r.value();
          remove_left(l,out);
          b->lock.unlock();
          return out;
        }
    }while(true);
}



#if 0

int 	jack_set_process_thread (jack_client_t *client, JackThreadCallback fun, void *arg)
int
main (int argc, char *argv[])
{
  const char **ports;
  const char *client_name = "simple";
  const char *server_name = NULL;
  jack_options_t options = JackNullOption;
  jack_status_t status;

  /* open a client connection to the JACK server */

  client = jack_client_open (client_name, options, &status, server_name);
  if (client == NULL) {
      fprintf (stderr, "jack_client_open() failed, "
                       "status = 0x%2.0x\n", status);
      if (status & JackServerFailed) {
          fprintf (stderr, "Unable to connect to JACK server\n");
        }
      exit (1);
    }
  if (status & JackServerStarted) {
      fprintf (stderr, "JACK server started\n");
    }
  if (status & JackNameNotUnique) {
      client_name = jack_get_client_name(client);
      fprintf (stderr, "unique name `%s' assigned\n", client_name);
    }

  /* tell the JACK server to call `process()' whenever
       there is work to be done.
    */

  jack_set_process_callback (client, process, 0);

  /* tell the JACK server to call `jack_shutdown()' if
       it ever shuts down, either entirely, or if it
       just decides to stop calling us.
    */

  jack_on_shutdown (client, jack_shutdown, 0);

  /* display the current sample rate.
     */

  printf ("engine sample rate: %" PRIu32 "\n",
          jack_get_sample_rate (client));

  /* create two ports */

  input_port = jack_port_register (client, "input",
                                   JACK_DEFAULT_AUDIO_TYPE,
                                   JackPortIsInput, 0);
  output_port = jack_port_register (client, "output",
                                    JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput, 0);

  if ((input_port == NULL) || (output_port == NULL)) {
      fprintf(stderr, "no more JACK ports available\n");
      exit (1);
    }

  /* Tell the JACK server that we are ready to roll.  Our
     * process() callback will start running now. */

  if (jack_activate (client)) {
      fprintf (stderr, "cannot activate client");
      exit (1);
    }

  /* Connect the ports.  You can't do this before the client is
     * activated, because we can't make connections to clients
     * that aren't running.  Note the confusing (but necessary)
     * orientation of the driver backend ports: playback ports are
     * "input" to the backend, and capture ports are "output" from
     * it.
     */

  ports = jack_get_ports (client, NULL, NULL,
                          JackPortIsPhysical|JackPortIsOutput);
  if (ports == NULL) {
      fprintf(stderr, "no physical capture ports\n");
      exit (1);
    }

  if (jack_connect (client, ports[0], jack_port_name (input_port))) {
      fprintf (stderr, "cannot connect input ports\n");
    }

  free (ports);

  ports = jack_get_ports (client, NULL, NULL,
                          JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL) {
      fprintf(stderr, "no physical playback ports\n");
      exit (1);
    }

  if (jack_connect (client, jack_port_name (output_port), ports[0])) {
      fprintf (stderr, "cannot connect output ports\n");
    }

  free (ports);

  /* keep running until stopped by the user */

  sleep (-1);

  /* this is never reached but if the program
       had some other way to exit besides being killed,
       they would be important to call.
    */

  jack_client_close (client);
  exit (0);
}

jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int
process (jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *in, *out;

  in = jack_port_get_buffer (input_port, nframes);
  out = jack_port_get_buffer (output_port, nframes);
  memcpy (out, in,
          sizeof (jack_default_audio_sample_t) * nframes);

  return 0;
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
  exit (1);
}



#endif
