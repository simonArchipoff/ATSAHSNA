#pragma once
#include <AudioIO.h>
#include "concurrentqueue.h"
#include <Analysis.h>
#include <RingBuffer.h>
#include <VectorPool.h>
#include <AudioIO.h>



template<typename T>
class Receiver {
public:
  Receiver(const VCD & signal, int number_output, double threshold=0.98)
    :dc(signal),ringBuffers(number_output),time_waited(0),size(signal.size()){
    threshold_level=threshold;
    for(auto &i : ringBuffers){
      i.reset(3*signal.size());
    }
  }

  struct result {
    template<typename iterator>
    result(T level, iterator begin, iterator end, int time):level(level),signal(begin,end),time(time){}
    T level = 0.0;
    std::vector<T> signal;
    int time;
  };

  bool tryGetResult(struct result & r){
    internalResult ir;
    if(resultsQueue.try_dequeue(ir)){
      r.level = ir.level;
      r.result = std::vector<T>(ir.data.begin(), ir.data.end());
      r.time = ir.time;
      vectorQueue.enqueue(ir.data.data());
      return true;
    }
    return false;
  }

  

  void rt_process(const AudioIO<T> & inputs){
    for(int i = 0; i<inputs.size(); i++){
      auto input = inputs[i];
      int frames = inputs[i].size();

      auto rb = ringBuffers[i];
      if(rb.freespace() < frames){
	rb.pop(frames-rb.freespace());
      }
      rb.write(input);
      time_waited += frames;

      if(rb.available() >= 2*size){
	auto tab = rb.read(2*size);
	auto r = dc.getDelays(array_VD_to_VCD(tab));
	//send result

	if(r.second > threshold_level){
	  auto time = r.first + time_waited - rb.available(); //r.first + time_waited - p.dc.getSize();
          internalResult result(r.second, tab.begin() + r.first
				,tab.begin() + r.first + size
				,time);
	  
	} else {
	  rb.pop(input.size());
	}
      }
    }
  }
private:
  VectorPool<T> pool;

  std::vector<RingBuffer<T>> ringBuffers;

  int time_waited;
  const DelayComputer dc;
  const T threshold_level;
  const uint timeout;
  const int size;
  
  struct internalResult {
    VectorCStyle<T> data;
    T level;
    int time;
  };

  void putVectorBack(T * v){
    vectorQueue.enqueue(v);
  }
  void putVectorBack(internalResult&r){
    putVectorBack(r.data);
  }
  moodycamel::ConcurrentQueue<T*> vectorQueue;
  moodycamel::ConcurrentQueue<internalResult> resultsQueue;
};

