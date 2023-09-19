#include "QJackView.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QValidator>
#include <QSpinBox>
#include <QFormLayout>



QJackView::QJackView(QWidget * parent)
    : QWidget{parent}
    ,inputButton{new QPushButton{tr("new input"),this}}
    ,outputButton{new QPushButton{tr("new output"),this}}
    ,sampleRate{new QLabel{this}}
    ,bufferSize{new QLabel{this}}
    ,inputName{new QLineEdit{this}}
    ,outputName{new QLineEdit{this}}
    ,gain{new QDoubleSpinBox{this}}
    ,portManager{new QJackPortManager{this}}
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

    l->addRow(tr("ports"),portManager);

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

void QJackView::addPort(jack_port_id_t port, QString portName){
    portManager->add(port,portName);
}
void QJackView::removePort(jack_port_id_t port){
    portManager->remove(port);
}

void QJackView::connectPort(jack_port_id_t a, jack_port_id_t b, QString namea, QString nameb){
    (void)namea;
    portManager->connectPort(a,b,nameb);

}
void QJackView::disconnectPort(jack_port_id_t a, jack_port_id_t b){
    portManager->disconnectPort(a,b);
}

/*
QBackends::QBackends(QWidget * parent)
  :QTabWidget{parent} {
  setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}

*/

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




QJackPortView::QJackPortView(QWidget *parent) : QWidget(parent) {
    name = new QLabel(this);
    nameConnexion = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(name);
    layout->addWidget(nameConnexion);

    setLayout(layout);
}

void QJackPortView::setName(QString portName) {
    name->setText("Port Name: " + portName);
}

void QJackPortView::setConnexionName(QString connName) {
    nameConnexion->setText("Connection Name: " + connName);
}



QJackPortManager::QJackPortManager(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    setLayout(layout);
}

QJackPortManager::~QJackPortManager() {
    for (auto it = portMap.begin(); it != portMap.end(); ++it) {
        QJackPortView *view = it.value();
        delete view;
    }
    portMap.clear();
}

void QJackPortManager::add(jack_port_id_t port, QString portName) {
    QJackPortView *view = new QJackPortView(this);
    portMap[port] = view;
    view->setName(portName);
    layout->addWidget(view);

}

void QJackPortManager::remove(jack_port_id_t port) {
    if (portMap.contains(port)) {
        QJackPortView *view = portMap[port];
        delete view;
        portMap.remove(port);
    }
}

QJackPortView* QJackPortManager::getPort(jack_port_id_t port) {
    if (portMap.contains(port)) {
        return portMap[port];
    }
    return nullptr;
}

void QJackPortManager::connectPort(jack_port_id_t a, jack_port_id_t b,QString nameb){
    auto pa = getPort(a);
    auto name = nameb;
    if(pa){
        pa->setConnexionName(name);
    }
}
void QJackPortManager::disconnectPort(jack_port_id_t a, jack_port_id_t b){
    auto pa = getPort(a);
    auto name = "";
    if(pa){
        pa->setConnexionName(name);
    }
}
