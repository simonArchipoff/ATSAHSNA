#pragma once

#include "qlineedit.h"
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


class QBackend {
public:
  QBackend(Backend * b = nullptr):backend{b}{};

  void setBackend(Backend * b){
    this->backend.reset(b);
  }
  Backend * getBackend(){
    return backend.data();
  }
protected:
  QScopedPointer<Backend> backend;
};


class QFaustDsp : public QWidget, public QBackend {
    Q_OBJECT
public:
    explicit QFaustDsp(BackendFaustQT * b, QWidget *parent = nullptr);

    void setUI(QWidget * ui);
    void setErrorMessage(QString);
    void compile();
    void setFaustCode(QString code,uint sampleRate);
private:
    QWidget * dspUi;
    QLineEdit * sr;
    QTextEdit * codeEdit;
    QLabel * errorLabel;
    QPushButton * compile_button;
    QVBoxLayout *layout;
};


/*
class QJackPort : public QWidget {
  Q_OBJECT
public:
  QJackPort(QWidget * parent=nullptr);
protected:
};
*/

/*
class QBackendJack : public QWidget, public QBackend {
  Q_OBJECT
public:
  QBackendJack(BackendJackQt * b, QWidget * parent=nullptr);

protected:

  void set_sample_rate(uint);
  void set_buffer_size(uint);


  QPushButton * inputButton, * outputButton;
  QLabel * sampleRate, *bufferSize;
  QLineEdit * inputName, *outputName;
  QSpinBox * latency;
  QDoubleSpinBox * gain;
};
*/

class QBackends : public QTabWidget {
public:
  QBackends(QWidget * parent=nullptr);

  BackendFaustQT * addFaustBackend();
  //BackendJackQt * addJackBackend();

  Backend * getSelectedBackend();
protected:
  vector<QBackend *> backends;
};




