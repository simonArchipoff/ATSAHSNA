#pragma once

#include <sys/types.h>
#include <Harmonics.h>
#include <Response.h>
#include <vector>
#include "concurrentqueue.h"

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

protected:
    ParamHarmonics paramHarmonics;
    ParamResponse paramResponse;
};


class MeasureSync{
    const vector<ResultHarmonics> getResultHarmonics();
    const vector<ResultResponse>  getResultResponse();

};



