#pragma once
#include <Sender.h>
#include <Receiver.h>

using namespace std;

class Acquisition{
public:
  Acquisition(const VCF & signal, SenderMode mode, int sender_number, int sender_timeoff
          , int receiver_number_input, uint receiver_timeout)
        :sender(array_complex_to_real<float,double>(signal), mode, sender_number, sender_timeoff)
        ,receiver(signal,receiver_number_input){
  }

  void rt_process(const AudioIO<float> & inputs, AudioIO<float> & outputs){
    sender.rt_output(outputs);
    //append_to_file("/tmp/output",outputs[0].v, outputs[0].size());
    receiver.rt_process(inputs);
    //append_to_file("/tmp/input",inputs[0].v, inputs[0].size());
  }

  const vector<float> & getSignal(){
   return sender.getSignal();
  }

  bool tryGetResult(ReceiverResult & r){
    return receiver.tryGetResult(r);
  }


protected:
  Sender sender;
  Receiver receiver;
};
