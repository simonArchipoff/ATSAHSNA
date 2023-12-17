#pragma once
#include <Sender.h>
#include <Receiver.h>

using namespace std;

template<typename T>
class Acquisition{
public:
  Acquisition(const VCD & signal, SenderMode mode, int sender_number, int sender_timeoff
	      , int receiver_number_output, int receiver_timeout)
    :sender(signal, mode, sender_number, sender_timeoff)
    ,receiver(signal,receiver_number_output){
  }

  void rt_process(const AudioIO<T> & inputs, AudioIO<T> & outputs){
    sender.rt_process(outputs);
    receiver.rt_process(inputs);
  }
  

  struct result{
    vector<T> result;
    int time;
    int input_index;
    int output_index;
  };
  
protected:
  Sender<T> sender;
  Receiver<T> receiver;  
};
