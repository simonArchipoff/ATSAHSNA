#include "delegate.h"

#include "Response.h"
#include "backend.h"
#include "delegate.moc"

#include "mainwindow.h"
#include "Harmonics.h"

#include <faust/gui/QTUI.h>

#include <BodePlot.h>
#include <qnamespace.h>
#ifdef ENABLE_JACK
#include "Jack.h"
#include "QJackView.h"
#endif
delegate::delegate(MainWindow * m):mw{m}
{
#ifdef ENABLE_JACK
    connect(m,&MainWindow::addJackBackendRequested,this, &delegate::addJackBackend);
#endif
    connect(m,&MainWindow::addFaustBackendRequested,this, &delegate::addFaustBackend);
    connect(m,&MainWindow::AddFaustBackendRequested,this, &delegate::addFaustBackendWithFile);
}

QBackendFaust::QBackendFaust(QFaustView * gui, QString name):QObject{},faust_gui{gui}{
    backend = new BackendFaust(name.toStdString());
    connectGUI();
}
QBackendFaust::~QBackendFaust(){
    delete backend;
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

#ifdef ENABLE_JACK
QBackendJack::QBackendJack(QJackView * gui, QString name):backend(new QJack()),jack_gui{gui}{
    connect(jack_gui,&QJackView::requestNewInputPort, this,[this](QString s){backend->addInputPort(s.toStdString());});
    connect(jack_gui,&QJackView::requestNewOutputPort,this,[this](QString s){backend->addOutputPort(s.toStdString());});

    connect(backend,&QJack::jack_samplerate_s,jack_gui,&QJackView::set_sample_rate);
    connect(backend,&QJack::jack_buffer_size_s,jack_gui,&QJackView::set_buffer_size);

    connect(backend,&QJack::jack_port_registration_s,this,[this](jack_port_id_t port, int i, QString name){
        if(i)
            jack_gui->addPort(port,name);
        else
            jack_gui->removePort(port);
    });

    connect(backend,&QJack::jack_port_connect_s, this,[this](jack_port_id_t a, jack_port_id_t b, int connect, QString namea, QString nameb){
        if(connect)
            jack_gui->connectPort(a,b,namea, nameb);
        else
            jack_gui->disconnectPort(a,b);
    });
//    void jack_port_connect_s(jack_port_id_t a, jack_port_id_t b, int connect, QString nameb);


    backend->start();
    connect(jack_gui,&QJackView::requestResponse,this,[this](auto p, auto c, auto i){
        this->backend->startResponse(p,c,i);
        });
}
QBackendJack::~QBackendJack(){
    delete backend;
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

void QBackendJack::timerEvent(QTimerEvent * e){
    auto r = backend->getResultResponse();
    if(!std::holds_alternative<std::monostate>(r))
        emit resultResponse(r);
    auto h = backend->getResultHarmonics();
    if(!std::holds_alternative<std::monostate>(h))
        emit resultHarmonics(h);
}

void delegate::addJackBackend(){
    auto j = mw->backends->addJack();
    jack = new QBackendJack(j,"jack");
    //j->set_sample_rate(jack->getSampleRate()); // bit ugly, I dont know why the callback isn't called
}
#endif
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
    auto fb = new QBackendFaust(f, name);
    faust.push_back(fb);
    connect(fb,&QBackendFaust::resultResponse, mw->displays,&QResponseView::setResults);
    /*connect(fb, &QBackendFaust::resultHarmonics, h, &THDPlot::setResult,
            Qt::UniqueConnection);*/
}

void delegate::addFaustBackendWithFile(QString path) {
    QString name = "faust" + QString::number(faust.size());
    auto f = mw->backends->addFaust(name);
    auto fb = new QBackendFaust(f, name);
    faust.push_back(fb);
    connect(fb,&QBackendFaust::resultResponse, mw->displays,&QResponseView::setResults);
    f->setFile(path);
    /*connect(fb, &QBackendFaust::resultHarmonics, h, &THDPlot::setResult,
            Qt::UniqueConnection);*/
}

void delegate::addResponseDisplay(){
    if(!mw->displays->isBodeInit()){
        auto bode = mw->displays->getBodePlot();

    }
}
void delegate::addHarmonicsDisplay(){
    /*if(!mw->displays->isTHDinit()){
        auto thd = mw->displays->getTHDPlot();
    }*/
}
void delegate::addSpectrogramDisplay(){
    if(!mw->displays->isSpectrogramInit()){
        auto spectrogram = mw->displays->getSpectrogramPlot();
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
}
*/
