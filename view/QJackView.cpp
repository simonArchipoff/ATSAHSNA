#include "QJackView.h"
#include "Response.h"
#include "qboxlayout.h"
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
    ,xruns_label{new QLabel{this}}
    ,inputName{new QLineEdit{this}}
    ,outputName{new QLineEdit{this}}
    ,gain{new QDoubleSpinBox{this}}
    ,portManager{new QJackPortManager{this}}
{
    inputButton->setDisabled(true);
    outputButton->setDisabled(true);
    QFormLayout * l = new QFormLayout{this};
    status = new QLabel{this};
    status->setText(tr("disconnected"));
    connectionButton = new QPushButton(tr("connect"),this);

    auto serverName = new QLineEdit{this};
    serverName->setPlaceholderText(tr("server name"));
    serverName->setText(APPNAME);
    l->addRow(connectionButton,serverName);
    l->addRow(tr("status jack"),status);
    connect(connectionButton,&QPushButton::clicked,this,[this,serverName](bool b){
        auto s = serverName->text();
        if(s.size() == 0)
            s = APPNAME;
        status->setText(tr("connexion pending"));
        emit requestConnect(s);
    });

    l->addRow(tr("sample rate"),sampleRate);
    l->addRow(tr("buffer size"),bufferSize);
    xruns_label->setText("0");
    l->addRow("xruns",xruns_label);
    l->addRow(inputButton,inputName);
    l->addRow(outputButton,outputName);

    inputName->setText(tr("input"));
    outputName->setText(tr("output"));

    l->addRow(tr("gain"),gain);
    gain->setMaximum(20);
    gain->setMinimum(-40);
    gain->setValue(0);

    l->addRow(tr("ports"),portManager);




    paramresponsewidget = new ParamResponseWidget(this);
    l->addRow(tr("response"),paramresponsewidget);

    connect(paramresponsewidget,&ParamResponseWidget::paramResponseChanged,this,[this](const ParamResponse & p){
        emit requestResponse(p,false,10);
        });
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

void QJackView::updateLevels(Levels l){
    portManager->updateLevels(l);
    update();
}

void QJackView::connected(){
    status->setText(tr("connected"));
    connectionButton->setDisabled(true);
    inputButton->setDisabled(false);
    outputButton->setDisabled(false);
}


void QJackView::connexion_failed(QString s){
    connectionButton->setDisabled(false);
    status->setText(s);
}

void QJackView::xrun(){
    xruns++;
    QString str;
    str.setNum(xruns);
    xruns_label->setText(str);
}


void QJackView::displayError(ErrorBackend e){
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Error"));
    msgBox.setText(QString(tr("Error jack:")) + QString(e.message.c_str()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


QJackPortView::QJackPortView(QWidget *parent) : QWidget(parent) {
    name = new QLabel(this);
    nameConnexion = new QLabel(this);
    vumeter = new VUMeter(this);
    vumeter->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    vumeter->setFixedWidth(50);
    vumeter->setFixedHeight(10);
    QHBoxLayout * hlayout = new QHBoxLayout(this);
    setLayout(hlayout);
    hlayout->addWidget(vumeter);

    hlayout->addWidget(name);
    hlayout->addWidget(nameConnexion);

}
QJackPortView::~QJackPortView(){
}

void QJackPortView::setName(QString portName) {
    name->setText("Port Name: " + portName);
}

void QJackPortView::setConnexionName(QString connName) {
    nameConnexion->setText("Connection Name: " + connName);
}

void QJackPortView::setLevel(float l,float max){
    vumeter->updateLevel(l,max);
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
    portMapName[portName] = port;
    view->setName(portName);
    layout->addWidget(view);

}

void QJackPortManager::remove(jack_port_id_t port) {
    if (portMap.contains(port)){
        QJackPortView *view = portMap[port];
        erase_if(portMapName,[port](const QMap<QString, jack_port_id_t>::iterator it) { return it.value() == port; });
        portMap.remove(port);
        layout->removeWidget(view);
        //delete view;
    }
}

QJackPortView* QJackPortManager::getPort(jack_port_id_t port) {
    if (portMap.contains(port)) {
        return portMap[port];
    }
    return nullptr;
}

void QJackPortManager::updateLevels(Levels l){
    for(int i = 0; i < l.spectrum_size; i++){
        if(l.ids[i]){
            QString qs(l.ids[i]);
            portMap[portMapName[qs]]->setLevel(l.level[i],l.max[i]);
        }
    }
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
