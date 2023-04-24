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
    virtual int latency() const{
        return 0;
    }
    virtual double linearZone() const = 0;
    VD preprocess_input(const VD & in) const;
    vector<VD> preprocess_inputs(const vector<VD> & in) const;
    vector<VD> postprocess_output(const vector<VD> & out) const;
};



template<class C>
vector<typename C::Result> compute(Backend * b, typename C::Param p){
    auto input =
        b->preprocess_input(C{}.generate_data(p, b->getSampleRate()));
    vector<VD> inputs;
    for(uint i = 0; i < b->numberInput() ; i++)
        inputs.push_back(input);
    auto outputs = b->postprocess_output(b->acquisition(inputs));
    vector<typename C::Result> res;
    for(auto & o : outputs){
        res.push_back(C{}.computeResult(o,p,b->getSampleRate()));
    }
    return res;
}
