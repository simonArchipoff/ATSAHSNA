#pragma once
#include "qobjectdefs.h"
#define FAUSTFLOAT double
#include <QWidget>

#include <faust/dsp/llvm-dsp.h>
#include <faust/gui/QTUI.h>
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

protected:
    BackendFaust() = default;
    ~BackendFaust();
    bool setCode(std::string dspCode);
    dsp * dspInstance;
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


class BackendFaustQT :  public QObject, public BackendFaust {
  Q_OBJECT
public:
    BackendFaustQT(QWidget * parent=nullptr);
    ~BackendFaustQT();

    bool setCode(QString dspCode, uint sampleRate);
    QWidget * getUI();
    void timerEvent(QTimerEvent *event) override;
signals:
    void changed();

protected:
    QTGUI * ui;
    APIUI * apiui;
    DetectChange detectchange;
};



typedef std::variant<BackendFaustQT *, QString> dsp_or_error;

dsp_or_error
create_faust_qt(QString dspCode, int sampleRate, QWidget * parent=nullptr);
