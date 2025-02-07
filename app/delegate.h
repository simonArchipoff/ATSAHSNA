#pragma once

#include <mainwindow.h>
#include "Harmonics.h"

#include <Faust.h>
#include <qbackend.h>
#include <QWeakPointer>
#include <qtmetamacros.h>
#ifdef ENABLE_JACK
#include "QJack.h"
#include "QJackView.h"
#endif

class QBackendFaust :  public QObject {
    Q_OBJECT
public:
    QBackendFaust(QFaustView * gui, QString name);
    ~QBackendFaust();

    void connectGUI();

    bool setCode(QString dspCode, uint sampleRate);

    bool isReady() const;
    string getErrorMessage(){
        return backend->getErrorMessage();
    }
    void compute();

signals:
    void changed();
    void resultResponse(std::variant<const std::vector<ResultResponse>,   ErrorBackend>& response);
    void resultHarmonics(std::variant< const std::vector<ResultHarmonics>,ErrorBackend>& harmonics);

protected:
    void timerEvent(QTimerEvent * e) override;
    BackendFaust * backend;

    QFaustView * faust_gui;
    bool computeHarmonic=false,
         computeResponse=false;

};
#ifdef ENABLE_JACK
// TODO : factorize this
class QBackendJack : public QObject {
    Q_OBJECT
public:
    QBackendJack(QJackView * gui, QString name);
    virtual ~QBackendJack();
signals:
    void changed();
    void resultResponse(BackendJack::ResultResponseVar &);
    void resultHarmonics(BackendJack::ResultHarmonicsVar &);

protected:
    void timerEvent(QTimerEvent * e) override;
    QJack * backend;
    QJackView * jack_gui;
};
#endif

class soundFile;

class QSoundFile :  public QObject {
    Q_OBJECT
public:
    QSoundFile();
    ~QSoundFile();

    void setWindow(int start, int nb);
    void openFile(QString path);

    void setInput(int i);
    void unsetInput(int i);

    void requestResponse();
    void requestHarmonics();

signals:
    void newFileOpen(int number_channels, int number_frames, int sample_rate);
    void fileFailed(QString s);
    void resultResponse(std::variant<const std::vector<ResultResponse>,   ErrorBackend>& response);
    void resultHarmonics(std::variant< const std::vector<ResultHarmonics>,ErrorBackend>& harmonics);

private:
    soundFile * soundfile;
};


//typedef std::variant<faust_backend *, QString> dsp_or_error;

//dsp_or_error
//create_faust_qt(QString dspCode, int sampleRate, QWidget * parent=nullptr);


class delegate : public QObject{
    Q_OBJECT
public:

    delegate(MainWindow * mw);
    ~delegate();

    void addFaustBackend(QString path="");

    void addSoundFileBackend(QString path="");
    QVector<QSoundFile *> soundfile;


    void addResponseDisplay();
    void addHarmonicsDisplay();
    void addSpectrogramDisplay();

    QVector<QBackendFaust *> faust;
#ifdef ENABLE_JACK
    void addJackBackend();
    QBackendJack * jack;
#endif
    MainWindow * mw;
};

