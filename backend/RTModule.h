#pragma once

#include "../constants.h"
#include "RingBuffer.h"
#include <Analysis.h>
#include <Harmonics.h>
#include <Response.h>
#include <tuple>
#include <variant>
#include "../helpers.h"
#include <set>
#include <memory>
#include "Response.h"
#include "concurrentqueue.h"


class RTModule {
public:
    virtual void rt_process(const vector<VD> & inputs, vector<VD> & outputs) = 0;
    virtual void rt_after_process(){};
};

class RTModuleResponse;
class RTModuleHarmonics;

class RTModuleHandler{
public:
    void setModule(std::shared_ptr<RTModule>);
    void startResponse(ParamResponse p,bool continuous,int integration);
    void startHarmonics();

    bool getResultHarmonics(vector<ResultHarmonics>& result);
    bool getResultResponse(vector<ResultResponse>& result);

protected:
    void setSampleRate(uint);

    void rt_process(const vector<VD> & inputs, vector<VD> & outputs);
    void rt_after_process();

    moodycamel::ConcurrentQueue<std::shared_ptr<RTModule>> toRTQueue;

    std::shared_ptr<RTModule> module;

private:
    void rt_updateModule();
    uint sampleRate = 0;
    std::shared_ptr<RTModuleResponse> responseRTModule;
};



class Sender {
public:
    enum Mode {All,RoundRobin};
    Sender(const VD& signal, Sender::Mode m, int number, int timeoff)
        : state(Timeoffing), mode(m), signal(signal), number_rec(number), timeoff(timeoff),
        current_output(0), current_send(0), current_timeoff(0), current_number_rec(0) {}

    void rt_output(int time, float ** output, int nb_output, int nb_frames);

private:
    int rt_timeoff(int start_idx, float * output, int nb_frames);
    int rt_send(int start_idx, float * output, int nb_frames);

private:
    enum State { Sending, SendingFinished, Timeoffing, TimeoffFinished, Finished};
//those two states are transitory ^^^                       ^^^
//they are used for the round robin logic
    State state;
    Mode mode;
    const VD signal;
    const int number_rec;
    const int timeoff;
    int current_output;
    int current_send;
    int current_timeoff;
    int current_number_rec;
};


template<typename T>
class VectorPool{
    VectorPool():pool(){}
    void init(int number, int size){
        for(auto i : pool){
            delete[] i;
        }
        pool.resize(number);
        for(auto & i : pool){
            i=new double[size];
        }
    }
    ~VectorPool(){
        for(auto i : pool){
            delete[] i;
        }
    }
    T * getVector(){
        for(auto & i : pool){
            if(i){
                auto tmp = i;
                i = nullptr;
                return tmp;
            }
        }
        return nullptr;
    }
    void putVectorBack(T *v){
        for(auto & i : pool){
            if(!i){
                i = v;
                return;
            }
        }
    }
private:
    std::vector<T *> pool;
};

class Receiver {
public:

    Receiver(const VD& signal, int timeout);


    void rt_input(){
    }

    struct ReceiverResult {
        double * data;
        int size;
        int time;
    };
    void putVectorBack(double * v){
        vectorQueue.enqueue(v);
    }
    void putVectorBack(ReceiverResult&r){
        putVectorBack(r.data);
    }

private:
    moodycamel::ConcurrentQueue<double*> vectorQueue;
    moodycamel::ConcurrentQueue<ReceiverResult> resultQueue;
    VectorPool<double> pool;
};



class Acquisition{
public:
    Acquisition():state(DISABLED){
    }

    struct timeout{
        int dummy;
    };

    struct result {
        double level = 0.0;
        VD result;
        int delay;
    };

    typedef std::variant<result,std::monostate,timeout> ret_type;

    void init(const VCD & signal,double threshold=0.9);

    void start(){
        state = SEND | RECIEVE;
        time_waited = 0;
        sending_index = 0;
        rb.reset();
    }

    ret_type rt_process(const VD & input, VD & output);


protected:
    struct params {
        VD signal;
        DelayComputer dc;
        double threshold_level;
        uint timeout;
        int sample_between_iteration;
    };


    enum state_t {
         DISABLED = 0
        ,SEND     = 2
        ,RECIEVE  = 4
    };

    uint state = SEND|RECIEVE;
    uint sending_index;

    void rt_process_sending(VD & input);
    uint time_waited;
    Acquisition::ret_type rt_process_wait_response(const VD & output);

    RingBuffer<double> rb;
    params p;
};



class RTModuleResponse : public RTModule {
public:
    RTModuleResponse(uint sampleRate, ParamResponse p, int integration_number = 1);

    void startResponse();
    bool tryGetResponse(ResultResponse & v);

    void setContinuous(bool);
    void setIntegrationSize(int s=1);

    virtual void rt_process(const vector<VD> & inputs, vector<VD> & outputs) override;
    virtual void rt_after_process() override;

private:
    VCD chirp;
    Acquisition acq;
    uint sampleRate;
    ParamResponse paramResponse;
    bool continuous;
    int integration_number;
    moodycamel::ConcurrentQueue<Acquisition::result> responseQueue;
    //accumulate results
    Accumulate<VD,double> acc;
};

