#pragma once
#include <Sender.h>
#include <Receiver.h>

using namespace std;

class Acquisition{
public:
  Acquisition(const VCD & signal, SenderMode mode, int sender_number, int sender_timeoff
          , int receiver_number_output, uint receiver_timeout)
        :sender(array_complex_to_real<float,double>(signal), mode, sender_number, sender_timeoff)
        ,receiver(signal,receiver_number_output){
  }

  void rt_process(const AudioIO<float> & inputs, AudioIO<float> & outputs){
    sender.rt_output(outputs);
    receiver.rt_process(inputs);
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
