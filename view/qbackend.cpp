#include "qbackend.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QValidator>
#include <QSpinBox>
#include <QFormLayout>

QFaustDsp::QFaustDsp(QWidget *parent)
    : QGroupBox{"faust",parent},
      dspUi{nullptr},
      codeEdit{new QTextEdit{this}},
      errorLabel{new QLabel{this}},
      layout{new QVBoxLayout{this}}
{
    this->setLayout(layout);
    layout->addWidget(codeEdit,5);
    sr = new QLineEdit(this);
    layout->addWidget(sr,1);
    sr->setValidator(new QIntValidator(1,1000000,this));
    sr->setText("44100");



    compile_button = new QPushButton(tr("valider"),this);
    //compile_button->isCheckable();
    layout->addWidget(compile_button,0);
    layout->addWidget(errorLabel,1);

    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
   // b->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::MinimumExpanding);
    codeEdit->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    errorLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    //codeEdit->setSizeAdjustPolicy(QTextEdit::AdjustToContents);

    codeEdit->setText(
    "import(\"filters.lib\");\n" \
    "fc = hslider(\"frequency\",100,10,20000,1);\n" \
    "q = hslider(\"quality\",1,0.1,10,0.1);\n" \
    "process=fi.resonlp(fc,q,1);\n");
    connect(compile_button,&QPushButton::clicked,this, &QFaustDsp::compile);
    connect(codeEdit,&QTextEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
    connect(sr,&QLineEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
    setMaximumWidth(minimumSizeHint().width());
}

void QFaustDsp::compile(){
  emit faustDspCode(codeEdit->toPlainText(),sr->text().toInt());
}

void QFaustDsp::setUI(QWidget * ui)
{
    if(dspUi == nullptr) {
        dspUi = ui;
    } else {
        layout->removeWidget(dspUi);
        dspUi = ui;
        ui->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    }
    layout->addWidget(ui,5);
    compile_button->setDisabled(true);
    setMaximumWidth(minimumSizeHint().width());
}

void QFaustDsp::setErrorMessage(QString s)
{
    compile_button->setDisabled(false);
    errorLabel->setText(s);
}




QBackendJack::QBackendJack(QWidget * parent)
  : QGroupBox{"jack",parent}
  ,inputButton{new QPushButton{tr("new input"),this}}
  ,outputButton{new QPushButton{tr("new output"),this}}
  ,inputName{new QLineEdit{this}}
  ,outputName{new QLineEdit{this}}
{
  QFormLayout * l = new QFormLayout{this};
  l->addRow(inputButton,inputName);
  l->addRow(outputButton,outputName);
  inputName->setText(tr("input"));
  outputName->setText(tr("output"));
  latency = new QSpinBox;

  latency->setRange(0,15000);
  latency->setValue(0);

  l->addRow(tr("latency"),latency);

  connect(latency,QOverload<int>::of(&QSpinBox::valueChanged),this,[this](int i){emit QBackendJack::newLatency(i);});

  setLayout(l);
  //setMaximumWidth(minimumSizeHint().width());
  connect(inputButton,&QPushButton::clicked,this,[this](){emit newInputPort(inputName->text());});
  connect(outputButton,&QPushButton::clicked,this,[this](){emit newOutputPort(outputName->text());});
}
