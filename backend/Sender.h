#pragma once
#include <vector>
#include <cassert>
#include <algorithm>
#include <AudioIO.h>


enum SenderMode {All,RoundRobin};


class Sender {
 public:
  template<typename U>
  Sender(const U& signal, SenderMode m, int number, int timeoff)
    : state(Timeoffing), mode(RoundRobin), signal(signal), number_rec(number), timeoff(timeoff),
    current_output(0), current_send(0), current_timeoff(0), current_number_rec(0) {}

  void rt_output(AudioIO<float> output){
    assert(output.size() > 0);
    assert(output[0].size()>0);
    auto * out = &output[current_output];
    int idx = 0;
    while(idx < out->size()){
      switch(state){
      case Sending:
          idx = rt_send(idx, out->data() ,out->size());
          assert(idx <= out->size());
	break;
      case SendingFinished:
	if(mode == RoundRobin){
	  current_output = current_output + 1 % output.size();
      out = &output[current_output];

	}
	if((current_output == 0 || mode == All) && ++current_number_rec == number_rec){
	  state = Finished;
	} else {
	  state = Timeoffing;
	  current_timeoff=0;
	}
	break;

      case Timeoffing:
    idx = rt_timeoff(idx,out->data(),out->size());
	break;
      case TimeoffFinished:
	state = Sending;
	current_send = 0;
	break;
      case Finished:
	return;
	break;
      }
    }

    if(mode == All){
      for(auto i : output){
        if(&i != out)
            std::copy(out->begin(), out->end(), i.begin());
        }
    }
  }


 private:
  int rt_timeoff(int start_idx, float * output, int nb_frames){
    assert(state == Timeoffing);
    assert(start_idx < nb_frames);
    assert(current_timeoff <= timeoff);
    (void)output;
    const auto current_timeoff_left = timeoff - current_timeoff;
    if( current_timeoff_left > nb_frames - start_idx){
      current_timeoff = nb_frames - start_idx;
      return nb_frames;
    } else {
      state = TimeoffFinished;
      return start_idx + current_timeoff_left;
    }
  }
  
  int rt_send(int start_idx, float * output, int nb_frames){
    assert(state == Sending);
    assert(current_number_rec < number_rec);
    const int signalsize = static_cast<int>(signal.size());
    assert(start_idx < nb_frames);
    assert(current_send < signalsize);
    const auto current_sending_left = signalsize - current_send;
    const auto left = nb_frames - start_idx;
    if(current_sending_left <= left){
      std::transform(signal.begin() + current_send
		     ,signal.end()
		     ,output + start_idx
		     ,[](double s){return static_cast<float>(s);});

      state = SendingFinished;
      return start_idx + left;
    } else {// currend_sending_left > left
      std::transform(signal.begin() + current_send
		     ,signal.begin() + current_send + left
		     ,output + start_idx
		     ,[](double s){return static_cast<float>(s);});
      current_send += left;

      assert(start_idx + left == nb_frames);
      return start_idx + left;
    }
  }

 private:
  enum State { Sending, SendingFinished, Timeoffing, TimeoffFinished, Finished};
  //those two states are transitory ^^^                       ^^^
  //they are used for the round robin logic
  State state;
  SenderMode mode;
  const std::vector<float> signal;
  int number_rec;
  int timeoff;
  int current_output;
  int current_send;
  int current_timeoff;
  int current_number_rec;
};
