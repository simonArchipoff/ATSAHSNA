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

#include "backend/backendJack.h"


class QFaustDsp : public QWidget
{
    Q_OBJECT
public:
    explicit QFaustDsp(QWidget *parent = nullptr);

    void setUI(QWidget * ui);
    void setErrorMessage(QString);
    void compile();
signals:
    void faustDspCode(QString code,uint sampleRate);
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

class QBackendJack : public QWidget{
  Q_OBJECT
public:
  QBackendJack(QWidget * parent=nullptr);

signals:
  void newInputPort(QString);
  void newOutputPort(QString);
  void newLatency(uint i);
protected:
  QPushButton * inputButton, * outputButton;
  QLineEdit * inputName, *outputName;
  QSpinBox * latency;
};

