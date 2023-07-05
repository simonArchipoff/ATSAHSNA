#pragma once

#include <mainwindow.h>
#include "signalHarmonics.h"
#include <backendFaust.h>
#include <qbackend.h>
#include <QWeakPointer>



class faust_backend  :  public QObject {
    Q_OBJECT
public:
    faust_backend(QSharedPointer<QFaustDsp> gui);
    ~faust_backend();

    void connectGUI();

    bool setCode(QString dspCode, uint sampleRate);

    bool isReady() const;
    string getErrorMessage(){
        return backend->getErrorMessage();
    }

signals:
    void changed();
    void resultResponse(std::variant<const std::vector<ResultResponse>>& response);
    void resultHarmonics(std::variant< const std::vector<ResultHarmonics>>& harmonics);

protected:
    void timerEvent(QTimerEvent * e) override;
    QScopedPointer<BackendFaust> backend;
    QWeakPointer<QFaustDsp> faust_gui;
};

//typedef std::variant<faust_backend *, QString> dsp_or_error;

//dsp_or_error
//create_faust_qt(QString dspCode, int sampleRate, QWidget * parent=nullptr);


class delegate : public QObject
{
    Q_OBJECT
public:

    delegate(MainWindow * mw);


    void addFaustBackend();
    void addJackBackend();

    QScopedPointer<faust_backend> faust;
    MainWindow * mw;
};

