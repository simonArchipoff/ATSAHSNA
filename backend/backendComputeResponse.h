#pragma once

#include "BackendRT.h"
#include "backendRingBuffer.h"
#include "signalResponse.h"

struct request  {ParamResponse param;};
struct response {ResultResponse response;};

class BackendComputeResponse : public RTModuleWithControl<request,response> {
public:

    virtual void rt_process(int nframe, const vector<float *> & inputs, vector<float *> & outputs)= 0;
    virtual void rt_after_process() = 0;

    typedef request Request;
    typedef response Response;

protected:
    virtual void rt_handleRequest(Request &r);
    virtual void handleResponse(Response &);

private:
    ParamResponse param;
    VCD chirp;
    VD chirp_real;
    RingBuffer<float> rb;
    FDF response;
};
