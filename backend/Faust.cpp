#include "Faust.h"
#include <faust/dsp/llvm-dsp.h>
#include <list>
#include <iostream>
#include <mutex>
#include <Generation.h>
#include <variant>

using namespace std;
BackendFaust::~BackendFaust(){
    if(dspInstance)
        delete dspInstance;
    if(factory)
        deleteDSPFactory(factory);
}


inline bool file_exists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

BackendFaust * make_faust_backend(ParamFaust p){
    BackendFaust * f = new BackendFaust(p.name);
    f->setCode(p.file_or_code,p.sample_rate);
    if(f->isReady()){
        for(auto & p : p.params){
            f->setParamValue(p.first,p.second);
        }
        return f;
    } else {
        std::cerr << f->getErrorMessage() << std::endl;
        delete f;
        return nullptr;
    }

}



bool BackendFaust::setCode(std::string dspCode_or_file, int sampleRate=DEFAULTSR){
    const char* argv[] = {"--double-precision-floats"};
    const int argc = sizeof(argv)/sizeof(*argv);
    // compiling in memory (createDSPFactoryFromFile could be used alternatively)

    factory = file_exists(dspCode_or_file) ?
                  createDSPFactoryFromFile(dspCode_or_file, argc, argv ,"", errorString,0)
                                           :  createDSPFactoryFromString("", dspCode_or_file, argc, argv ,"", errorString,0);

    dspInstance = factory ? factory->createDSPInstance() : nullptr;
    if(dspInstance){
        dspInstance->init(sampleRate);
        dspInstance->buildUserInterface(&apiui);
        detectChange.setAPIUI(&apiui);
    }
    return dspInstance;
}

uint BackendFaust::getSampleRate() const{
    return dspInstance->getSampleRate();
}

bool BackendFaust::isReady() const {
    return dspInstance;
}
void BackendFaust::init(uint sampleRate){
    dspInstance->init(sampleRate);
}

std::string BackendFaust::getErrorMessage(){
    return errorString;
}

uint BackendFaust::numberInput()const {
    return dspInstance->getNumInputs();
}
uint BackendFaust::numberOutput() const{
    return dspInstance->getNumOutputs();
}

void BackendFaust::setParamValue(std::string name, FAUSTFLOAT value){
    assert(dspInstance);
    apiui.setParamValue(name.c_str(),value);
}

//send a bunch of 0, I dont know how to properly init internal state
void reinit(BackendFaust * b,int size){
    VD zero(size,0.0);
    vector<VD>in(b->numberInput(), zero);
    (void)b->acquisition(in);
}

vector<VD> BackendFaust::acquisition(const vector<VD> &in){
    vector<VD> input{in};
    assert(input.size() > 0);
    uint inputSize = input[0].size();

    vector<double*> inputs{numberInput()};
    for(uint i = 0; i < inputs.size(); i++){
        inputs[i] = input[i].data();
    }
    vector<double*> outputs{numberOutput()};
    vector<vector<double>> out{numberOutput()};

    for(uint i = 0; i < outputs.size(); i++){
        out[i].resize(inputSize);
        outputs[i] = out[i].data();
    }
    for(uint i = 0; i < outputs.size(); i++){
        for(uint j = 0; j < inputSize; j++){
            outputs[i][j] = ~0;
        }
    }

    uint block = 4*1024;
    uint nb_block = inputSize / block;

    for(uint i = 0; i < nb_block; i++){
        dspInstance->compute(block,inputs.data(), outputs.data());
        for(uint j = 0; j < outputs.size(); j++){
            outputs[j] = outputs[j] + block;
        }
        for(uint j = 0; j < inputs.size(); j++){
            inputs[j] = inputs[j] + block;
        }
    }

    dspInstance->compute(inputSize % block,inputs.data(), outputs.data());

    return out;
}


bool BackendFaust::didSomethingChanged(){
    if(dspInstance){
        reinit(this,1);
        return detectChange.isSomethingChanged();
    }
    return false;
}

std::variant<const std::vector<ResultResponse>> BackendFaust::getResultResponse(){
    const std::lock_guard<std::mutex> g(this->lock);
    dspInstance->instanceClear();
    //reinit(this,getSampleRate());
    std::vector<ResultResponse> res;
    auto in = impulse(paramResponse.freqMin, paramResponse.duration, getSampleRate());
    auto out = acquisition(vector<VD>(numberInput(),in));
    int i=0;
    for(auto & o : out){
        auto tmp = computeResponse(paramResponse,in, o, getSampleRate());
        tmp.name = nameInstance + "_" +std::to_string(i++) ;
        res.push_back(tmp);
    }
    return std::variant<const std::vector<ResultResponse>>(res);

}
std::variant<const std::vector<ResultHarmonics>>  BackendFaust::getResultHarmonics(){
    const std::lock_guard<std::mutex> g(this->lock);
    dspInstance->instanceClear();
    vector<ResultHarmonics> res;
    auto in = sinusoid(paramHarmonics.frequency, 1, getSampleRate());
    auto out = acquisition(vector<VD>(numberInput(),in));
    for(auto & o : out){
        res.push_back(computeTHD(paramHarmonics,o, getSampleRate()));
    }
    return std::variant<const std::vector<ResultHarmonics>>(res);
}

void BackendFaust::buildUserInterface(UI * ui){
    dspInstance->buildUserInterface(ui);
}

DetectChange::DetectChange(){}
void DetectChange::setAPIUI(APIUI * ui){
    faustZones.resize(0);
    ref.resize(0);
    for(int i = 0; i < ui->getParamsCount(); i++){
        faustZones.push_back(ui->getParamZone(i));
        ref.push_back(0);//ui->getParamValue(i));
    }
}

bool DetectChange::isSomethingChanged(){
    bool changed=false;
    for(uint i = 0; i<faustZones.size(); i++){
        changed |= *faustZones[i] != ref[i];
        ref[i] = *faustZones[i];
    }
    return changed;
}

