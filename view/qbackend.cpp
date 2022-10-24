#include "qbackend.h"
#include "backendJack.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QValidator>
#include <QSpinBox>
#include <QFormLayout>

QFaustDsp::QFaustDsp(BackendFaustQT * backend, QWidget *parent)
    : QWidget{parent}
      ,QBackend{backend}
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
    //setMaximumWidth(minimumSizeHint().width());
}

void QFaustDsp::compile(){
  setFaustCode(codeEdit->toPlainText(),sr->text().toInt());
}


void QFaustDsp::setFaustCode(QString code,uint sampleRate){
    auto res = create_faust_qt(code,sampleRate,this);
    try{
        auto tmp = std::get<BackendFaustQT*>(res);
        backend.reset(tmp);
        this->setErrorMessage("");
        this->setUI(static_cast<BackendFaustQT*>(backend.data())->getUI());
    } catch (const std::bad_variant_access& ex) {
        auto s = std::get<QString>(res);
        this->setErrorMessage(s);
    }
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
//setMaximumWidth(minimumSizeHint().width());
}

void QFaustDsp::setErrorMessage(QString s)
{
    compile_button->setDisabled(false);
    errorLabel->setText(s);
}


QBackendJack::QBackendJack(BackendJackQt * b, QWidget * parent)
  : QWidget{parent}
    ,QBackend{b}
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
  connect(gain,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,[b](auto d){b->setOutputGain(d);});

  connect(latency,QOverload<int>::of(&QSpinBox::valueChanged),this,[b](int i){b->setLatency(i);});
  connect(b,&BackendJackQt::new_latency ,latency, &QSpinBox::setValue);

  connect(automatic_latency,&QCheckBox::stateChanged,this,[this,b](int i){
      latency->setDisabled(i);
      b->setLatencyAutomatic(i);
    });

  setLayout(l);
  //setMaximumWidth(minimumSizeHint().width());
  connect(inputButton,&QPushButton::clicked,this,[b,this](){b->addInputPort(inputName->text().toStdString());});
  connect(outputButton,&QPushButton::clicked,this,[b,this](){b->addOutputPort(outputName->text().toStdString());});
  QString str;
  str.setNum(b->getSampleRate());
  sampleRate->setText(str);

  str.setNum(b->getBufferSize());
  bufferSize->setText(str);

  connect(b,&BackendJackQt::buffer_size_change,this,&QBackendJack::set_buffer_size);
  connect(b,&BackendJackQt::sample_rate_change,this,&QBackendJack::set_sample_rate);
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



QBackends::QBackends(QWidget * parent)
  :QTabWidget{parent} {
  setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}


void QBackends::addFaustBackend(){
  auto b = new BackendFaustQT{this};
  auto bv = new QFaustDsp{b};
  addTab(bv,"faust");
  backends.push_back(bv);
  bv->compile();
}

void QBackends::addJackBackend(){
  auto b = new BackendJackQt;
  auto bv = new QBackendJack{b,this};
  addTab(bv,"jack");
  backends.push_back(bv);
}

Backend * QBackends::getSelectedBackend(){
  auto i = this->currentIndex();
  if(i < 0)
    return nullptr;
  assert(static_cast<uint>(i) < backends.size());
  return backends[i]->getBackend();
}
