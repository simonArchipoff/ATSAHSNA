#pragma once

#include <mainwindow.h>
#include "Harmonics.h"
#include <Faust.h>
#include <qbackend.h>
#include <QWeakPointer>



class faust_backend  :  public QObject {
    Q_OBJECT
public:
    faust_backend(QFaustView * gui, QString name);
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
    BackendFaust * backend;
    QFaustView * faust_gui;
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

    void addResponseDisplay();
    void addHarmonicsDisplay();

    QVector<faust_backend *> faust;
    QJack * jack;
    MainWindow * mw;
};

