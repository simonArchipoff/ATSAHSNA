#pragma once
#include "concurrentqueue.h"
#include <Analysis.h>
#include <RingBuffer.h>
#include <VectorPool.h>
#include <AudioIO.h>



class Receiver {
public://this code look the signal in a window of twice it sice, this is sub optimal, the better would be, i think, signal size + buffer size
       // anyway, this is why this code has some "size*2" everywhere, inside DelayComputer as well, this should be made more explicit.
    Receiver(const VCD & signal, int number_output, double threshold=0.98)
        :pool(32,signal.size()), tmp(signal.size() * 2 - 1), dc(signal,signal.size()),threshold_level(threshold),ringBuffers(number_output),time_waited(0),size(signal.size()){
        for(auto &i : ringBuffers){
            i.reset(3*signal.size());
        }
    }

    struct result {
        template<typename iterator>
        result(float level, iterator begin, iterator end, int time):level(level),signal(begin,end),time(time){}
        float level = 0.0;
        std::vector<float> signal;
        int time;
    };

    bool tryGetResult(struct result & r){
        internalResult ir;
        if(resultsQueue.try_dequeue(ir)){
            r.level = ir.level;
            //r.data = std::vector<float>(ir.data.begin(), ir.data.end());
            r.time = ir.time;
            vectorQueue.enqueue(ir.data.data());
            return true;
        }
        return false;
    }

    void rt_process(const AudioIO<float> & inputs){
        for(int i = 0; i<inputs.size(); i++){
            auto input = inputs[i];
            int frames = inputs[i].size();

            auto & rb = ringBuffers[i];
            if(rb.freespace() < frames){
                rb.pop(frames-rb.freespace());
            }
            rb.write(input.data(),input.size());
            time_waited += frames;

            if(rb.available() >= 2*size - 1){
                rb.read(2*size - 1 ,tmp.data());

#pragma warning "this is not rt"
                auto r = dc.getDelay(tmp.data(),tmp.size());
                if(r.second > threshold_level){
                    rb.pop(2*size);
                    auto time = r.first + time_waited - rb.available(); //r.first + time_waited - p.dc.getSize();
                    internalResult result;
                    result.data = pool.getVector();
                    result.level = r.second;
                    result.time = time;
                    std::copy(tmp.begin() + r.first
                              ,tmp.begin() + r.first + size
                              ,result.data.begin());
                    resultsQueue.enqueue(result);

                } else {
                    rb.pop(input.size());
                }
            }
        }
    }
private:
    VectorPool<float> pool;
    std::vector<float> tmp;
    std::vector<RingBuffer<float>> ringBuffers;

    int time_waited;
    DelayComputer dc;
    const float threshold_level;
    const uint timeout = 10000;
    const int size;

    struct internalResult {
        VectorCStyle<float> data;
        float level;
        int time;
    };

    void putVectorBack(float * v){
        vectorQueue.enqueue(v);
    }
    void putVectorBack(internalResult&r){
        putVectorBack(r.data.data());
    }
    moodycamel::ConcurrentQueue<float*> vectorQueue;
    moodycamel::ConcurrentQueue<internalResult> resultsQueue;
};

