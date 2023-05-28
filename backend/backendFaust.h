#pragma once

#define FAUSTFLOAT double
#include <cstdint> //hopefully this header wont be necessary, it's a workaround because it misses in dsp.h
#include <faust/dsp/llvm-dsp.h>
#include <cstring> //missing header in APIUI.h
#include <faust/gui/APIUI.h>
#include <mutex>
#include "backend.h"

using namespace std;

struct ParamFaust {
    string file_or_code;
    uint sample_rate;
    vector<pair<string,double>> params;
};

class BackendFaust : public Backend {
public:
    string getErrorMessage();
    uint numberInput()   const override;
    uint numberOutput()  const override;
    uint getSampleRate() const override;
    bool isReady()       const override;
    vector<VD> acquisition(const vector<VD> &input);
    BackendFaust(){}
    ~BackendFaust();
    bool setCode(string dspCode,int sampleRate);

    //if something goes wrong APIUI write something on stderr
    void setParamValue(string name, FAUSTFLOAT value);


    variant<vector<ResultHarmonics>> getResultHarmonics() override;
    variant<vector<ResultResponse>>  getResultResponse()   override;


protected:
    mutex lock;
    dsp * dspInstance;
    APIUI  apiui;
    string errorString;
    llvm_dsp_factory * factory;
};

BackendFaust * make_faust_backend(ParamFaust p);

class DetectChange {
public:
    DetectChange(){}
    DetectChange(APIUI*ui){
        for(int i = 0; i < ui->getParamsCount(); i++){
            faustZones.push_back(ui->getParamZone(i));
            ref.push_back(ui->getParamValue(i));
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
