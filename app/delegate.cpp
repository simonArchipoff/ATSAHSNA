 #include "delegate.h"
#include "mainwindow.h"
#include "signalHarmonics.h"
#include <faust/gui/QTUI.h>

#include <BodePlot.h>

delegate::delegate(MainWindow * m):mw{m}
{
    connect(m,&MainWindow::addJackBackendRequested,this, &delegate::addJackBackend);
    connect(m,&MainWindow::addFaustBackendRequested,this, &delegate::addFaustBackend);
}

faust_backend::faust_backend(QSharedPointer<QFaustDsp> gui):QObject{},faust_gui{gui}{
    backend.reset(new BackendFaust);
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
    connect(faust_gui.toStrongRef().data(),&QFaustDsp::setFaustCode,
            this,&faust_backend::setCode);

}

bool faust_backend::setCode(QString dspCode, uint sampleRate){
    if(backend->setCode(dspCode.toStdString(),sampleRate)){
        QTGUI * ui = new QTGUI{nullptr};
        ui->setParent(faust_gui.toStrongRef().data());
        backend->buildUserInterface((QTGUI*)ui);
        faust_gui.toStrongRef()->setDSPUI(ui);
        backend->init(sampleRate);
        startTimer(100);
        return true;
    }
    faust_gui.toStrongRef().data()->setErrorMessage(QString(backend->getErrorMessage().c_str()));
    return false;
}


bool faust_backend::isReady() const{
    return backend->isReady();
}



void delegate::addFaustBackend(){
    auto f = mw->backends->addFaust();
    auto d = mw->displays->addBodePlot();
    faust.reset(new faust_backend{f});
    connect(faust.data()
            ,&faust_backend::resultResponse
            ,d.data()
            ,&BodePlot::setResponses
            ,Qt::UniqueConnection);
}

void delegate::addJackBackend(){
    auto j = mw->backends->addJack();
    jack.reset(new BackendJack);
    connect(j.data(),&QBackendJack::requestNewInputPort, this,[this](QString s){jack->addInputPort(s.toStdString());});
    connect(j.data(),&QBackendJack::requestNewOutputPort,this,[this](QString s){jack->addOutputPort(s.toStdString());});
}

void delegate::addResponseDisplay(){
    if(!mw->displays->isBodeInit()){
        auto bode = mw->displays->addBodePlot();

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
