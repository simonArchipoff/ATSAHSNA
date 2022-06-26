#pragma once
#define FAUSTFLOAT double
#include <QWidget>

#include <faust/dsp/llvm-dsp.h>
#include <faust/gui/QTUI.h>

#include "backend.h"



class BackendFaust : public Backend
{
public:
    std::string getErrorMessage();
    uint numberInput() const  override;
    uint numberOutput() const override ;
    uint getSampleRate() const override ;
    bool isReady() const override ;
    vector<VD> acquisition(const vector<VD> &input) override;

protected:
    BackendFaust() = default;
    ~BackendFaust();
    bool setCode(std::string dspCode);
    dsp * dspInstance;
    std::string errorString;
    llvm_dsp_factory * factory;
};

class BackendFaustQT : public BackendFaust{
public:
    BackendFaustQT(QWidget * parent=nullptr);
    ~BackendFaustQT();

    bool setCode(QString dspCode, uint sampleRate);
    QWidget * getUI();
protected:
    QTGUI * ui;
};



typedef std::variant<BackendFaustQT *, QString> dsp_or_error;

dsp_or_error
create_faust_qt(QString dspCode, int sampleRate, QWidget * parent=nullptr);
