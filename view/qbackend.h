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

class QBackendJack : public QWidget, public QBackend {
  Q_OBJECT
public:
  QBackendJack(BackendJack * b, QWidget * parent=nullptr);

protected:
  QPushButton * inputButton, * outputButton;
  QLabel * sampleRate;
  QLineEdit * inputName, *outputName;
  QSpinBox * latency;
};


class QBackends : public QTabWidget {
public:
  QBackends(QWidget * parent=nullptr);

  void addFaustBackend();
  void addJackBackend();

  Backend * getSelectedBackend();
protected:
  vector<QBackend *> backends;
};
