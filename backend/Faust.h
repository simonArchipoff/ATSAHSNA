#pragma once


#define FAUSTFLOAT double
#include <cstring>

#include <faust/dsp/llvm-dsp.h>
#include <faust/gui/APIUI.h>
#include <mutex>
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
    DetectChange();
    void setAPIUI(APIUI*ui);

    bool isSomethingChanged();

private:

    vector<FAUSTFLOAT *> faustZones;
    vector<FAUSTFLOAT> ref;
    bool firstTime = true;
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

    variant<const vector<ResultHarmonics>,ErrorBackend> getResultHarmonics();
    variant<const vector<ResultResponse>, ErrorBackend>  getResultResponse();
    void buildUserInterface(UI * ui);
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




