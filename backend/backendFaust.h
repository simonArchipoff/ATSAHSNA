#pragma once

#define FAUSTFLOAT double
#include <faust/dsp/llvm-dsp.h>
#include <cstring> //missing header in APIUI.h
#include <faust/gui/APIUI.h>

#include "backend.h"


class BackendFaust : public Backend
{
public:
    std::string getErrorMessage();
    uint numberInput()   const override;
    uint numberOutput()  const override;
    uint getSampleRate() const override;
    bool isReady() const override;
    vector<VD> acquisition(const vector<VD> &input) override;

    BackendFaust() = default;
    ~BackendFaust();
    bool setCode(std::string dspCode,int sampleRate);
protected:
    dsp * dspInstance;
    APIUI * apiui;
    std::string errorString;
    llvm_dsp_factory * factory;
};


class DetectChange {
public:
  DetectChange(){};
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

