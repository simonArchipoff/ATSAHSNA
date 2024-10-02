#pragma once

#include <sys/types.h>
#include <Harmonics.h>
#include <Response.h>
#include <vector>
#include <variant>


using std::variant;


class ErrorBackend {
public:
    std::string message;
    ErrorBackend(const std::string& msg) : message(msg) {}
    ErrorBackend():ErrorBackend("unspecified error"){}
};


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
    //variant<const vector<ResultHarmonics>> getResultHarmonics();
    //variant<const vector<ResultResponse>>  getResultResponse();

protected:
    ParamHarmonics paramHarmonics;
    ParamResponse paramResponse;
};


class MeasureSync{
    vector<ResultHarmonics> getResultHarmonics();
    vector<ResultResponse>  getResultResponse();
};

class MeasureASync {
    const vector<ResultHarmonics> getResultHarmonics();
    const vector<ResultResponse>  getResultResponse();
};
