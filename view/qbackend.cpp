#include "qbackend.h"
#include "backendJack.h"
#include "qmeasure.h"
#include "qnamespace.h"
#include "qpushbutton.h"

#include <faust/gui/QTUI.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QValidator>
#include <QSpinBox>
#include <QFormLayout>

QFaustDsp::QFaustDsp(QWidget *parent)
    : QWidget{parent}
    ,dspUi{nullptr}
    ,codeEdit{new QTextEdit{this}}
    ,errorLabel{new QLabel{this}}
    ,layout{new QVBoxLayout{this}}
{
    this->setLayout(layout);
    layout->addWidget(codeEdit,5);
    sr = new QLineEdit(this);
    layout->addWidget(sr,1);
    sr->setValidator(new QIntValidator(1,1000000,this));
    sr->setText("44100");

    compile_button = new QPushButton(tr("valider"),this);
    layout->addWidget(compile_button,0);
    layout->addWidget(errorLabel,1);

    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    codeEdit->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    errorLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    codeEdit->setText(
        "import(\"filters.lib\");\n" \
        "fc = hslider(\"frequency\",100,10,20000,1);\n" \
        "q = hslider(\"quality\",1,0.1,10,0.1);\n" \
        "process=fi.resonlp(fc,q,1);\n");
    connect(compile_button,&QPushButton::clicked,this, &QFaustDsp::compile);
    connect(codeEdit,&QTextEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
    connect(sr,&QLineEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
}

void QFaustDsp::compile(){
    emit setFaustCode(codeEdit->toPlainText(),sr->text().toInt());
}

void QFaustDsp::setDSPUI(QWidget * ui)
{
    if(dspUi == nullptr) {
        dspUi.reset(ui);
    } else {
        layout->removeWidget(dspUi.data());
        dspUi.reset(ui);
        ui->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    }
    layout->addWidget(dspUi.data(),5);
    compile_button->setDisabled(true);
    errorLabel->setText("");
}

void QFaustDsp::setErrorMessage(QString s)
{
    compile_button->setDisabled(false);
    errorLabel->setText(s);
}


QBackendJack::QBackendJack(QWidget * parent)
    : QWidget{parent}
    ,inputButton{new QPushButton{tr("new input"),this}}
    ,outputButton{new QPushButton{tr("new output"),this}}
    ,sampleRate{new QLabel{this}}
    ,bufferSize{new QLabel{this}}
    ,inputName{new QLineEdit{this}}
    ,outputName{new QLineEdit{this}}
    ,gain{new QDoubleSpinBox{this}}
{
    QFormLayout * l = new QFormLayout{this};
    l->addRow(tr("sample rate"),sampleRate);
    l->addRow(tr("buffer size"),bufferSize);
    l->addRow(inputButton,inputName);
    l->addRow(outputButton,outputName);

    inputName->setText(tr("input"));
    outputName->setText(tr("output"));
    latency = new QSpinBox;

    auto automatic_latency = new QCheckBox;
    latency->setRange(0,15000);
    latency->setValue(0);
    l->addRow(tr("automatic latency"),automatic_latency);
    l->addRow(tr("latency"),latency);
    l->addRow(tr("gain"),gain);
    gain->setMaximum(20);
    gain->setMinimum(-40);
    gain->setValue(0);
    //connect(gain,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,[b](auto d){b->setOutputGain(d);});

    //connect(latency,QOverload<int>::of(&QSpinBox::valueChanged),this,[b](int i){b->setLatency(i);});
    //connect(b,&BackendJackQt::new_latency ,latency, &QSpinBox::setValue);
    /*
  connect(automatic_latency,&QCheckBox::stateChanged,this,[this,b](int i){
      latency->setDisabled(i);
      b->setLatencyAutomatic(i);
    });
*/
    automatic_latency->setCheckState(Qt::CheckState::Checked);

    setLayout(l);
    //setMaximumWidth(minimumSizeHint().width());
    //connect(inputButton,&QPushButton::clicked,this,[b,this](){b->addInputPort(inputName->text().toStdString());});
    //connect(outputButton,&QPushButton::clicked,this,[b,this](){b->addOutputPort(outputName->text().toStdString());});
    QString str;
    //str.setNum(b->getSampleRate());
    sampleRate->setText(str);

    //str.setNum(b->getBufferSize());
    bufferSize->setText(str);

    //connect(b,&BackendJack::buffer_size_change,this,&QBackendJack::set_buffer_size);
    //connect(b,&BackendJack::sample_rate_change,this,&QBackendJack::set_sample_rate);
}

void QBackendJack::set_sample_rate(uint n){
    QString str;
    str.setNum(n);
    sampleRate->setText(str);
}
void QBackendJack::set_buffer_size(uint n){
    QString str;
    str.setNum(n);
    bufferSize->setText(str);
}


/*
QBackends::QBackends(QWidget * parent)
  :QTabWidget{parent} {
  setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}

*/
std::list<GUI*> GUI::fGuiList;
/*
BackendFaustQT * QBackends::addFaustBackend(){
  auto b = new BackendFaustQT{this};
  auto bv = new QFaustDsp{b};
  addTab(bv,"faust");
  fausts.push_back(bv);
  bv->compile();
  return b;
}
*/
/*
BackendJackQt * QBackends::addJackBackend(){
  auto b = new BackendJackQt;
  auto bv = new QBackendJack{b,this};
  addTab(bv,"jack");
  backends.push_back(bv);
  return b;
}
*/
/*
Backend * QBackends::getSelectedBackend(){
  auto i = this->currentIndex();
  if(i < 0)
    return nullptr;
  assert(static_cast<uint>(i) < fausts.size());
  return fausts[i]->getBackend();
}

*/


