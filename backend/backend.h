#pragma once

#include <sys/types.h>
#include <signalHarmonics.h>
#include <signalResponse.h>
#include <variant>
#include <vector>

class Backend {
public:
    virtual ~Backend() = default;
    virtual uint numberInput() const = 0;
    virtual uint numberOutput() const = 0;
    virtual uint getSampleRate() const = 0;
    virtual bool isReady() const = 0;


    void setParamHarmonics(ParamHarmonics p){
        paramHarmonics = p;
    }
    void setParamResponse(ParamResponse p){
        paramResponse = p;
    }

    virtual std::variant<std::vector<ResultHarmonics>> getResultHarmonics() = 0;
    virtual std::variant<std::vector<ResultResponse>>  getResultResponse()  = 0;

protected:
    ParamHarmonics paramHarmonics;
    ParamResponse paramResponse;
};




/*
void setLatency(int l);
uint getLatence() const{
    return latency;
}*/
