#pragma once

#include "AudioIO.h"
#include <Analysis.h>
#include <jack/types.h>
#include <cstring>
#include "concurrentqueue.h"


struct Levels{
    static const int spectrum_size = 256;
    static constexpr const float coef = 0.9;
    Levels(){
        reset();
    }
    void reset(){
        for(int i = 0; i < spectrum_size; i++){
            ids[i] = nullptr;
            level[i] = 0.0;
        }
    }

    void update(const char * name, const VectorCStyle<float> &f){
        for(int i = 0; i < spectrum_size; i++){
            if(ids[i] == nullptr || (0 == strcmp(ids[i], name))){
                ids[i] = name;
                level[i] = std::max((1. -coef) * level[i] +  coef * 10 * log(rms_r(f.v, f.v + f.v_size)),-150.);
                auto m = std::minmax_element(f.v,f.v+f.v_size);
                max[i] = 10 * log(std::max(-(*m.first),*(m.second)));
                return;
            }
        }
    }

    float getLevel(const char * name){
        for(int i = 0; i < spectrum_size; i++){
            if(0==strcmp(ids[i],name)){
                return level[i];
            }
        }
        assert(false);
    }
    const char * ids[spectrum_size];
    float level[spectrum_size];
    float max[spectrum_size];

};


class LevelMonitor
{
public:
    LevelMonitor():l(){}

    bool getLevels(Levels &l){
        return level_queue.try_dequeue(l);
    }
    void observe(const char * name, const VectorCStyle<float> & f){
        l.update(name,f);
    }
protected:
    void send(){
        level_queue.enqueue(l);
    }

    Levels l;
    moodycamel::ConcurrentQueue<Levels> level_queue;
};
