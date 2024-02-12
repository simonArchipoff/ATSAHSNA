#pragma once
#include <Sender.h>
#include <Receiver.h>

using namespace std;

class Acquisition{
public:
  Acquisition(const VCD & signal, SenderMode mode, int sender_number, int sender_timeoff
	      , int receiver_number_output, int receiver_timeout)
        :sender(array_complex_to_real<float,double>(signal), mode, sender_number, sender_timeoff)
    ,receiver(signal,receiver_number_output){
  }

  void rt_process(const AudioIO<float> & inputs, AudioIO<float> & outputs){
    sender.rt_output(outputs);
    receiver.rt_process(inputs);
  }
  struct result;
  bool tryGetResult(result & r){

    abort();
  }

  struct result{
    vector<float> result;
    int time_sent;
    int time_result;
    int input_index;
    int output_index;
  };


protected:
  Sender sender;
  Receiver receiver;
};
