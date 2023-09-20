#pragma once

#define FAUSTFLOAT double
#include <faust/dsp/llvm-dsp.h>
#include <cstring>
#include <faust/gui/APIUI.h>
#include <mutex>
#include <variant>
#include "backend.h"

using namespace std;

struct ParamFaust {
    string name;
    string file_or_code;
    uint sample_rate;
    vector<pair<string,double>> params;
};

class DetectChange {
public:
    DetectChange(){}
    void setAPIUI(APIUI*ui){
        faustZones.resize(0);
        ref.resize(0);
        for(int i = 0; i < ui->getParamsCount(); i++){
            faustZones.push_back(ui->getParamZone(i));
            ref.push_back(0);//ui->getParamValue(i));
        }
    }

    bool isSomethingChanged(){
        bool changed=false;
        for(uint i = 0; i<faustZones.size(); i++){
            changed |= *faustZones[i] != ref[i];
            ref[i] = *faustZones[i];
        }
        return changed;
    }

private:

    vector<double *> faustZones;
    vector<double> ref;
};

class BackendFaust : public Backend {
public:
    string getErrorMessage();
    uint numberInput()   const override;
    uint numberOutput()  const override;
    uint getSampleRate() const override;
    bool isReady()       const override;

    vector<VD> acquisition(const vector<VD> &input);
    BackendFaust(string nameInstance):nameInstance(nameInstance){}
    ~BackendFaust();
    bool setCode(string dspCode,int sampleRate);
    void init(uint sampleRate);
    //if something goes wrong APIUI write something on stderr
    void setParamValue(string name, FAUSTFLOAT value);

    bool didSomethingChanged();

    variant<const vector<ResultHarmonics>> getResultHarmonics();
    variant<const vector<ResultResponse>>  getResultResponse();
    void buildUserInterface(UI * ui){
        dspInstance->buildUserInterface(ui);
    }

protected:
    mutex lock;
    dsp * dspInstance = nullptr;
    string nameInstance;
    APIUI  apiui;
    DetectChange detectChange;
    string errorString;
    llvm_dsp_factory * factory = nullptr;
};

BackendFaust * make_faust_backend(ParamFaust p);




