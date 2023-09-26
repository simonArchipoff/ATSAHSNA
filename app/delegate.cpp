#include "delegate.h"
#include "mainwindow.h"
#include "Harmonics.h"
#include <faust/gui/QTUI.h>

#include <BodePlot.h>

delegate::delegate(MainWindow * m):mw{m}
{
    connect(m,&MainWindow::addJackBackendRequested,this, &delegate::addJackBackend);
    connect(m,&MainWindow::addFaustBackendRequested,this, &delegate::addFaustBackend);
}

faust_backend::faust_backend(QFaustView * gui, QString name):QObject{},faust_gui{gui}{
    backend = new BackendFaust(name.toStdString());
    connectGUI();
}
faust_backend::~faust_backend(){
  //  ui->stop();
}
void faust_backend::timerEvent(QTimerEvent * e){
    if(backend->didSomethingChanged()){
        auto response = backend->getResultResponse();
        auto harmonics = backend->getResultHarmonics();
        emit resultResponse(response);
        emit resultHarmonics(harmonics);
    }
}

void faust_backend::connectGUI(){
    connect(faust_gui,&QFaustView::setFaustCode,
            this,&faust_backend::setCode);

}

bool faust_backend::setCode(QString dspCode, uint sampleRate){
    if(backend->setCode(dspCode.toStdString(),sampleRate)){
        QTGUI * ui = new QTGUI{nullptr};
        ui->setParent(faust_gui);
        backend->buildUserInterface((QTGUI*)ui);
        faust_gui->setDSPUI(ui);
        backend->init(sampleRate);
        startTimer(100*1./30);
        return true;
    }
    faust_gui->setErrorMessage(QString(backend->getErrorMessage().c_str()));
    return false;
}


bool faust_backend::isReady() const{
    return backend->isReady();
}



void delegate::addFaustBackend(){
    QString name = "faust" + QString::number(faust.size());
    auto f = mw->backends->addFaust(name);
    auto d = mw->displays->getBodePlot();
    auto fb = new faust_backend(f,name);
    faust.push_back(fb);
    connect(fb
            ,&faust_backend::resultResponse
            ,d
            ,&BodePlot::setResponses
            ,Qt::UniqueConnection);
}

void delegate::addJackBackend(){
    auto j = mw->backends->addJack();
    jack = new QJack;
    connect(j,&QJackView::requestNewInputPort, this,[this](QString s){jack->addInputPort(s.toStdString());});
    connect(j,&QJackView::requestNewOutputPort,this,[this](QString s){jack->addOutputPort(s.toStdString());});

    connect(jack,&QJack::jack_samplerate_s,j,&QJackView::set_sample_rate);
    connect(jack,&QJack::jack_buffer_size_s,j,&QJackView::set_buffer_size);

    connect(jack,&QJack::jack_port_registration_s,this,[j](jack_port_id_t port, int i, QString name){
        if(i)
            j->addPort(port,name);
        else
            j->removePort(port);
    });

    connect(jack,&QJack::jack_port_connect_s, this,[j](jack_port_id_t a, jack_port_id_t b, int connect, QString namea, QString nameb){
        if(connect)
            j->connectPort(a,b,namea, nameb);
        else
            j->disconnectPort(a,b);
    });
//    void jack_port_connect_s(jack_port_id_t a, jack_port_id_t b, int connect, QString nameb);


    jack->start();
    //j->set_sample_rate(jack->getSampleRate()); // bit ugly, I dont know why the callback isn't called
}

void delegate::addResponseDisplay(){
    if(!mw->displays->isBodeInit()){
        auto bode = mw->displays->getBodePlot();

    }
}
void delegate::addHarmonicsDisplay(){
    if(!mw->displays->isTHDinit()){
        auto thd = mw->displays->addTHDPlot();

    }

}


/*
std::variant<faust_backend *, QString>
create_faust_qt(QString dspCode, int sampleRate, QWidget * parent){
    auto tmp = new faust_backend(parent);
    tmp->setCode(dspCode, sampleRate);
    if(tmp->isReady())
        return dsp_or_error{tmp};
    QString s = QString::fromStdString(tmp->getErrorMessage());
    delete tmp;
    return dsp_or_error{s};
}*/
