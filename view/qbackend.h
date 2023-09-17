#pragma once

#include "Response.h"
#include "qlineedit.h"
#include "qmeasure.h"
#include "qscopedpointer.h"
#include <QSharedPointer>
#include "qwidget.h"
#include <QWidget>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>

#include <Jack.h>
#include <Faust.h>
#include <faust/gui/QTUI.h>





class QFaustView : public QWidget{
    Q_OBJECT
public:
    explicit QFaustView(QWidget *parent = nullptr);

    void setDSPUI(QWidget * ui);
    void setErrorMessage(QString);
    void compile();
signals:
    void setFaustCode(QString code, uint sampleRate);

public:
    QSharedPointer<QWidget> dspUi;
    QLineEdit * sr;
    QTextEdit * codeEdit;
    QLabel * errorLabel;
    QPushButton * compile_button;
    QVBoxLayout * layout;
};



class QJackView : public QWidget {
    Q_OBJECT
public:
    QJackView(QWidget * parent=nullptr);
    void set_sample_rate(uint);
    void set_buffer_size(uint);
signals:
    void requestNewInputPort(QString);
    void requestNewOutputPort(QString);

    void requestResponse(ParamResponse p, bool continuous, int integration=1);
    void requestHarmonics(ParamHarmonics p, bool continuous);



protected:

    QPushButton * inputButton, * outputButton;
    QLabel * sampleRate, *bufferSize;
    QLineEdit * inputName, *outputName;
    QDoubleSpinBox * gain;
};



class QBackends : public QTabWidget{
public:
    QBackends(QWidget * parents):QTabWidget(parents){
    }
    QSharedPointer<QFaustView> addFaust();
    QSharedPointer<QJackView> addJack();
protected:
    std::vector<QSharedPointer<QFaustView>> fausts;
    std::vector<QSharedPointer<QJackView>> jacks;
};
