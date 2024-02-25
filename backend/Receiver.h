#pragma once
#include "concurrentqueue.h"
#include <Analysis.h>
#include <RingBuffer.h>
#include <VectorPool.h>
#include <AudioIO.h>
#include <iostream>

struct ReceiverResult {
public:
    ReceiverResult(){}
    template<typename iterator>
    ReceiverResult(float level, iterator begin, iterator end, int time):level(level),signal(begin,end),time(time){}
    float level = 0.0;
    int idx;
    std::vector<float> signal;
    int time;
};

class Receiver {
public://this code look the signal in a window of twice it sice, this is sub optimal, the better would be, i think, signal size + buffer size
       // anyway, this is why this code has some "size*2" everywhere, inside DelayComputer as well, this should be made more explicit.
    Receiver(const VCD & signal, int number_output, double threshold=0.2)
        :pool(32,signal.size()), tmp(signal.size() * 2 - 1), ringBuffers(number_output),time_waited(0),dc(signal,signal.size()),threshold_level(threshold),size(signal.size()){
        for(auto &i : ringBuffers){
            i.reset(3*signal.size());
        }
    }

    bool tryGetResult(struct ReceiverResult & r){
        internalResult ir;
        if(resultsQueue.try_dequeue(ir)){
            r.level = ir.level;
            r.signal = std::vector<float>(ir.data.begin(), ir.data.end());
            r.time = ir.time;
            r.idx = ir.idx;
            vectorQueue.enqueue(ir.data.data());
            return true;
        }
        return false;
    }

    void rt_process(const AudioIO<float> & inputs){
        for(int i = 0; i<inputs.size(); i++){
            auto input = inputs[i];
            uint frames = inputs[i].size();

            auto & rb = ringBuffers[i];
            if(rb.freespace() < frames){
                rb.pop(frames-rb.freespace());
            }
            rb.write(input.data(),input.size());
            time_waited += frames;

            if(rb.available() >= 2*size - 1){
                rb.read(2*size - 1 ,tmp.data());
                auto r = dc.getDelay(tmp.data(),tmp.size());

                if(r.second > threshold_level && r.first >= 0){
                    auto time = r.first + time_waited - rb.available(); //r.first + time_waited - p.dc.getSize();
                    std::cerr << r.first << std::endl;
                    rb.pop(size);
                    internalResult result;
                    result.data = pool.getVector();
                    result.level = r.second;
                    result.idx = i;
                    result.time = time;
                    assert(tmp.size() >= r.first + size);

                    assert(result.data.size() <= size);
                    for(uint i = 0; i < size; i++){
                        assert(r.first + i < tmp.size());
                        result.data[i] = tmp[r.first + i];
                    }
#if 1
                        static int fooa = 0;
                    std::string path = "/tmp/s";
                        to_file(path+std::to_string(fooa)+"_" + std::to_string(r.second) + "_" + std::to_string(r.first),tmp.data() + r.first, size);
                    to_file(path+"foo"+std::to_string(fooa), rb.raw_read());
                    fooa++;
#endif
                    /*
                    std::copy(tmp.begin() + r.first
                             ,tmp.begin() + r.first + size
                             ,result.data.begin());
                    */
                    resultsQueue.enqueue(result);
                } else {
                    rb.pop(input.size());
                }
            }
        }
    }
    ~Receiver(){
        float * v;
        while(vectorQueue.try_dequeue(v)){
            pool.putVectorBack(v);
        }
        internalResult r;
        while(resultsQueue.try_dequeue(r)){
            pool.putVectorBack(r.data);
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
    const uint size;

    struct internalResult {
        VectorCStyle<float> data;
        float level;
        int idx;
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

