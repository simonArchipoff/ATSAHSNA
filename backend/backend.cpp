#include "backend.h"
#include "../helpers.h"
#include <assert.h>


VD Backend::preprocess_input(const VD & in) const {
    VD res{in};
    pad_right_0(latency(),res);
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

vector<VD> Backend::postprocess_output(const vector<VD> & out)const {
    vector<VD> res;
    for(auto & i : out){
        VD tmp = i;
        remove_left(latency(),tmp);
        res.push_back(tmp);
    }
    return res;
}
