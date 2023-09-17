#include "qbackend.h"
#include "qnamespace.h"
#include "qpushbutton.h"

#include <faust/gui/QTUI.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QValidator>
#include <QSpinBox>
#include <QFormLayout>

QFaustView::QFaustView(QWidget *parent)
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
        "fc = hslider(\"frequency\",100,100,20000,1);\n" \
        "q = hslider(\"quality\",1,0.1,10,0.1);\n" \
        "process=fi.resonlp(fc,q,1);\n");
    connect(compile_button,&QPushButton::clicked,this, &QFaustView::compile);
    connect(codeEdit,&QTextEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
    connect(sr,&QLineEdit::textChanged, this, [this](){compile_button->setDisabled(false);});
}

void QFaustView::compile(){
    emit setFaustCode(codeEdit->toPlainText(),sr->text().toInt());
}

void QFaustView::setDSPUI(QWidget * ui)
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

void QFaustView::setErrorMessage(QString s)
{
    compile_button->setDisabled(false);
    errorLabel->setText(s);
}


QJackView::QJackView(QWidget * parent)
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

    setLayout(l);
    //setMaximumWidth(minimumSizeHint().width());
    connect(inputButton,&QPushButton::clicked,this,[this](){emit requestNewInputPort(inputName->text());});
    connect(outputButton,&QPushButton::clicked,this,[this](){emit requestNewOutputPort(outputName->text());});
    QString str;
    //str.setNum(b->getSampleRate());
    sampleRate->setText(str);

    //str.setNum(b->getBufferSize());
    bufferSize->setText(str);

    //connect(b,&BackendJack::buffer_size_change,this,&QBackendJack::set_buffer_size);
    //connect(b,&BackendJack::sample_rate_change,this,&QBackendJack::set_sample_rate);
}

void QJackView::set_sample_rate(uint n){
    QString str;
    str.setNum(n);
    sampleRate->setText(str);
}
void QJackView::set_buffer_size(uint n){
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


QSharedPointer<QFaustView> QBackends::addFaust(){
    auto f = QSharedPointer<QFaustView>(new QFaustView);
    addTab(f.data(),"faust"+QString::number(fausts.size()));
    fausts.push_back(f);
    return f;
}


QSharedPointer<QJackView> QBackends::addJack(){
    auto f = QSharedPointer<QJackView>(new QJackView);
    addTab(f.data(),"jack"+QString::number(jacks.size()));
    jacks.push_back(f);
    return f;
}
