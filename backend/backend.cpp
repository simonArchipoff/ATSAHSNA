#include "backend.h"
#include <assert.h>





/*

void Backend::setLatency(int l){
    latency = l;
}
void Backend::setGainDb(double db){
    gain = pow(10,db/20);
}
void Backend::setGainFactor(double g){
    gain = g;
}


VD Backend::preprocess_input(const VD & in) const {
    VD res{in};
    pad_right_0(latency,res);
    return res;
}
vector<VD> Backend::preprocess_inputs(const vector<VD> & in) const {
    assert(in.size() > 0);
    vector<VD> res;
    for(auto & v : in){
        res.push_back(preprocess_input(v));
    }
    return res;
}

vector<VD> Backend::postprocess_output(const vector<VD> & out) const {
    vector<VD> res;
    for(auto & i : out){
        VD tmp = i;
        for(auto & i : tmp){
            i /= gain;
        }
        remove_left(latency,tmp);
        res.push_back(tmp);
    }
    return res;
}

*/
