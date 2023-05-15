#pragma once

#include "../constants.h"

class Backend {
public:
    virtual ~Backend() = default;
    virtual uint numberInput() const = 0;
    virtual uint numberOutput() const = 0;
    virtual uint getSampleRate() const = 0;
    virtual bool isReady() const = 0;
    virtual vector<VD> acquisition(const vector<VD> &input) = 0;

    void setLatency(int l);
    void setGainDb(double db);
    void setGainFactor(double g);

    VD preprocess_input(const VD & in) const;
    vector<VD> preprocess_inputs(const vector<VD> & in) const;
    vector<VD> postprocess_output(const vector<VD> & out) const;
protected:
    int latency = 0;
    double gain = 1.0;
};


