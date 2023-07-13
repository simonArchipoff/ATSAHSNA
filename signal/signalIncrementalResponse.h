#pragma once
#include <signalResponse.h>


class SignalIncrementalResponse
{

public:
    SignalIncrementalResponse(ParamResponse response);

private:
    ParamResponse response;
    ResultResponse result;
    VCD signal;

};


