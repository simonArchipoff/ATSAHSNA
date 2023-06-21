#pragma once

#include "qscopedpointer.h"
#include "qtmetamacros.h"
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
    void timerEvent(QTimerEvent *event) override;
    bool isReady() const;
    string getErrorMessage(){
        return backend->getErrorMessage();
    }
signals:
    void changed();

protected:
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

    delegate();


    void addFaustBackend(QSharedPointer<QFaustDsp> gui);
    void addJackBackend();

    QScopedPointer<faust_backend> faust;
};

