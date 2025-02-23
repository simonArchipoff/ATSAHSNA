#include "delegate.h"


#include "delegate.moc"

#include "mainwindow.h"

#include <faust/gui/QTUI.h>

#include <BodePlot.h>
#include <qnamespace.h>
#include <QtConcurrent/QtConcurrent>

#ifdef ENABLE_JACK
#include "QJack.h"
#include "QJackView.h"
#endif

#include <soundfile.h>

delegate::delegate(MainWindow * m):mw{m}
{
#ifdef ENABLE_JACK
    connect(m,&MainWindow::addJackBackendRequested,this, &delegate::addJackBackend);
#endif
    connect(m,&MainWindow::addFaustBackendRequested,this, &delegate::addFaustBackend);
    connect(m,&MainWindow::addSoundFileRequested, this, &delegate::addSoundFileBackend);
}

delegate::~delegate(){
    for(auto & i : faust){
        delete i;
    }
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
        compute();
    }
}

void QBackendFaust::connectGUI(){
    connect(faust_gui,&QFaustView::setFaustCode,
            this,&QBackendFaust::setCode);
    connect(faust_gui,&QFaustView::setHarmonic,this,[this](bool b){
        this->computeHarmonic=b;
        this->compute();
    });
    connect(faust_gui,&QFaustView::setResponse,this,[this](bool b){
        this->computeResponse=b;
        this->compute();
    });


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
        this->compute();
        return true;
    }
    faust_gui->setErrorMessage(QString(backend->getErrorMessage().c_str()));
    return false;
}


bool QBackendFaust::isReady() const{
    return backend->isReady();
}

void QBackendFaust::compute(){
    if(computeResponse){
        auto response = backend->getResultResponse();
        emit resultResponse(response);
    }
    if(computeHarmonic){
        auto harmonics = backend->getResultHarmonics();
        emit resultHarmonics(harmonics);
    }
}

#ifdef ENABLE_JACK
QBackendJack::QBackendJack(QJackView * gui, QString name):backend(new QJack(this)),jack_gui{gui}{
    connect(jack_gui,&QJackView::requestNewInputPort, this,[this](QString s){backend->addInputPort(s.toStdString());});
    connect(jack_gui,&QJackView::requestNewOutputPort,this,[this](QString s){backend->addOutputPort(s.toStdString());});

    connect(backend,&QJack::jack_samplerate_s,jack_gui,&QJackView::set_sample_rate);
    connect(backend,&QJack::jack_buffer_size_s,jack_gui,&QJackView::set_buffer_size);
    connect(backend,&QJack::jack_xrun_s, jack_gui, &QJackView::xrun);
    connect(backend,&QJack::jack_shutdown_s, jack_gui, &QJackView::shutdown);

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

    connect(backend,&QJack::levels,jack_gui,&QJackView::updateLevels);

    connect(jack_gui,&QJackView::requestResponse,this,[this](auto p, auto c, auto i){
        this->backend->startResponse(p,c,i);
        });
    connect(jack_gui, &QJackView::requestConnect, this, [this](auto s){
        QtConcurrent::run([this,s](){this->backend->start_jack(s.toStdString());});
    });
    connect(backend, &QJack::jack_started_s,        jack_gui, &QJackView::connected);
    connect(backend, &QJack::jack_started_failed_s, jack_gui, &QJackView::connexion_failed);
    connect(backend,&QJack::jack_request_failed, jack_gui,    &QJackView::displayError);

    startTimer(100*1./30);
    //backend->start();
    jack_gui->click_connect_button();
}
QBackendJack::~QBackendJack(){
    delete backend;
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };


void QBackendJack::timerEvent(QTimerEvent * e){
    auto r = backend->getResultResponse();
    if(!std::holds_alternative<ErrorBackend>(r))
        emit resultResponse(r);
    auto h = backend->getResultHarmonics();
    if(!std::holds_alternative<ErrorBackend>(h))
        emit resultHarmonics(h);
}

void delegate::addJackBackend(){
    auto j = mw->backends->addJack();
    jack = new QBackendJack(j,"jack");
    connect(jack,&QBackendJack::resultResponse, mw->response,&QResponseView::setResults);
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



void delegate::addFaustBackend(QString path) {
    QString name = "faust" + QString::number(faust.size());
    auto f = mw->backends->addFaust(name);
    auto fb = new QBackendFaust(f, name);
    faust.push_back(fb);
    connect(fb,&QBackendFaust::resultResponse, mw->response,&QResponseView::setResults);
    connect(fb,&QBackendFaust::resultHarmonics, mw->harmonic, &QHarmonicView::setResults);
    if(path!=""){
        f->setFile(path);
    }
    /*connect(fb, &QBackendFaust::resultHarmonics, h, &THDPlot::setResult,
            Qt::UniqueConnection);*/
}

void delegate::addSoundFileBackend(QString path){
    QString name = "audio" + QString::number(soundfile.size());
    auto f = mw->backends->addSoundFile();
    auto fb = new QSoundFile;
    soundfile.push_back(fb);
    connect(f,  &QSoundFileWidget::openFileRequested, fb, &QSoundFile::openFile);
    connect(fb, &QSoundFile::newFileOpen, f, &QSoundFileWidget::handleNewFileOpen);
    connect(fb, &QSoundFile::fileFailed, f, &QSoundFileWidget::handleFileFailed);
    connect(f,  &QSoundFileWidget::setInputChannelRequested, fb, &QSoundFile::setInput);
    connect(f,  &QSoundFileWidget::unsetInputChannelRequested, fb, &QSoundFile::unsetInput);
    connect(f,  &QSoundFileWidget::setWindowRequested, fb, &QSoundFile::setWindow);
    connect(fb, &QSoundFile::resultHarmonics, mw->harmonic, &QHarmonicView::setResults);
    connect(fb, &QSoundFile::resultResponse, mw->response, &QResponseView::setResults);
    if(path != ""){
        fb->openFile(path);
    }
}

void delegate::addResponseDisplay(){
    if(!mw->response->isBodeInit()){
        auto bode = mw->response->getBodePlot();
    }
}
void delegate::addHarmonicsDisplay(){
    /*if(!mw->displays->isTHDinit()){
        auto thd = mw->displays->getTHDPlot();
    }*/
}
void delegate::addSpectrogramDisplay(){
    if(!mw->response->isSpectrogramInit()){
        auto spectrogram = mw->response->getSpectrogramPlot();
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

QSoundFile::QSoundFile(){
    soundfile = new soundFile;
}

QSoundFile::~QSoundFile(){
    if(soundfile)
        delete soundfile;
}

void QSoundFile::setWindow(int start, int nb){
    soundfile->setWindow(start,nb);
    auto h = soundfile->getResultHarmonics();
    emit resultHarmonics(h);
    auto r = soundfile->getResultResponse();
    emit resultResponse(r);
}

void QSoundFile::openFile(QString path){
    soundfile->setFile(path.toStdString());
    if(soundfile->isReady()){
        emit newFileOpen(soundfile->numberChannels(), soundfile->numberFrames(), soundfile->getSampleRate());
    } else {
        emit fileFailed(QString::fromStdString(soundfile->error()));
    }
}

void QSoundFile::setInput(int i){
    soundfile->setInput(i);
}

void QSoundFile::unsetInput(int i){
    soundfile->unsetInput(i);
}
