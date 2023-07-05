#pragma once

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

#include <backendJack.h>
#include <backendFaust.h>
#include <faust/gui/QTUI.h>





class QFaustDsp : public QWidget{
    Q_OBJECT
public:
    explicit QFaustDsp(QWidget *parent = nullptr);

    void setDSPUI(QWidget * ui);
    void setErrorMessage(QString);
    void compile();
signals:
    void setFaustCode(QString code,uint sampleRate);

public:
    QSharedPointer<QWidget> dspUi;
    QLineEdit * sr;
    QTextEdit * codeEdit;
    QLabel * errorLabel;
    QPushButton * compile_button;
    QVBoxLayout * layout;
};



class QBackendJack : public QWidget {
  Q_OBJECT
public:
  QBackendJack(QWidget * parent=nullptr);

protected:

  void set_sample_rate(uint);
  void set_buffer_size(uint);

  QPushButton * inputButton, * outputButton;
  QLabel * sampleRate, *bufferSize;
  QLineEdit * inputName, *outputName;
  QSpinBox * latency;
  QDoubleSpinBox * gain;
};



class QBackends : public QTabWidget{
  public:
  QBackends(QWidget * parents):QTabWidget(parents){
  }
  QSharedPointer<QFaustDsp> addFaust();
  QSharedPointer<QBackendJack> addJack();
  protected:
  std::vector<QSharedPointer<QFaustDsp>> fausts;
  std::vector<QSharedPointer<QBackendJack>> jacks;
};
