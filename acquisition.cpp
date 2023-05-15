#include "acquisition.h"
#include "signalHarmonics.h"
#include "signalResponse.h"

/*
Acquisition::Acquisition(Backend * b):backend(b)
{

}
*/

/*
template<class C>
vector<typename C::Result> compute_helper(Backend * b, typename C::Param p){
    auto input = b->preprocess_input(C{}.generate_data(p, b->getSampleRate()));
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

template<typename K, typename T>
std::string new_string(const std::map<K,T> & m, const K & s){
    int n = 0;
    std::string k;
    do{
        k = s+std::to_string(n++);
    }while(m.count(k));
    return k;
}

std::string  Acquisition::compute(ParamResponse p ,std::string name){
    auto ns = new_string<std::string,vector<ResultResponse>>(responses,name);
    auto v = compute_helper<ImpulseResponse>(backend,p);
    responses.insert({ns,v});

    return ns;
}
std::string Acquisition::compute(ParamHarmonics p, std::string name){
    auto ns = new_string<std::string,vector<ResultHarmonics>>(harmonics,name);
    auto v = compute_helper<HarmonicResponse>(backend,p);
    harmonics.insert({ns,v});
    return ns;
}


*/
