#include "delegate.h"
#include "delegate.moc"
#include "Jack.h"
#include "mainwindow.h"
#include "Harmonics.h"

#include <faust/gui/QTUI.h>

#include <BodePlot.h>
#include <qnamespace.h>

delegate::delegate(MainWindow * m):mw{m}
{
    connect(m,&MainWindow::addJackBackendRequested,this, &delegate::addJackBackend);
    connect(m,&MainWindow::addFaustBackendRequested,this, &delegate::addFaustBackend);
}

QBackendFaust::QBackendFaust(QFaustView * gui, QString name):QObject{},faust_gui{gui}{
    backend = new BackendFaust(name.toStdString());
    connectGUI();
}
QBackendFaust::~QBackendFaust(){
  //  ui->stop();
}
void QBackendFaust::timerEvent(QTimerEvent * e){
    if(backend->didSomethingChanged()){
        auto response = backend->getResultResponse();
        auto harmonics = backend->getResultHarmonics();
                emit resultResponse(response);
        emit resultHarmonics(harmonics);
    }
}

void QBackendFaust::connectGUI(){
    connect(faust_gui,&QFaustView::setFaustCode,
            this,&QBackendFaust::setCode);

}

bool QBackendFaust::setCode(QString dspCode, uint sampleRate){
    if(backend->setCode(dspCode.toStdString(),sampleRate)){
        QTGUI * ui = new QTGUI{nullptr};
        ui->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
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


bool QBackendFaust::isReady() const{
    return backend->isReady();
}

QBackendJack::QBackendJack(QJackView * gui, QString name):backend(new BackendJack()),jack_gui{gui}{}
QBackendJack::~QBackendJack(){
    delete backend;
}


void QBackendJack::timerEvent(QTimerEvent * e){

}

/*
class QBackendJack : public QObject {
    Q_OBJECT
public:


signals:
    void changed();
    void resultResponse(std::variant<const std::vector<ResultResponse>>& response);
    void resultHarmonics(std::variant< const std::vector<ResultHarmonics>>& harmonics);

protected:
 
    BackendJack * backend;
    QBackendJack * jack_gui;
};
*/


void delegate::addFaustBackend() {
    QString name = "faust" + QString::number(faust.size());
    auto f = mw->backends->addFaust(name);
    auto d = mw->displays->getBodePlot();
    auto h = mw->displays->getTHDPlot();
    auto fb = new QBackendFaust(f, name);
    faust.push_back(fb);
    connect(fb, &QBackendFaust::resultResponse, d, &BodePlot::setResponses,
            Qt::UniqueConnection);
    connect(fb, &QBackendFaust::resultHarmonics, h, &THDPlot::setResult,
            Qt::UniqueConnection);
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
        auto thd = mw->displays->getTHDPlot();

    }

}


/*
std::variant<QBackendFaust *, QString>
create_faust_qt(QString dspCode, int sampleRate, QWidget * parent){
    auto tmp = new QBackendFaust(parent);
    tmp->setCode(dspCode, sampleRate);
    if(tmp->isReady())
        return dsp_or_error{tmp};
    QString s = QString::fromStdString(tmp->getErrorMessage());
    delete tmp;
    return dsp_or_error{s};
}*/
