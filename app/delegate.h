#pragma once

#include <mainwindow.h>
#include "Harmonics.h"
#include "Jack.h"
#include "QJackView.h"
#include <Faust.h>
#include <qbackend.h>
#include <QWeakPointer>
#include <qtmetamacros.h>



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

signals:
    void changed();
    void resultResponse(std::variant<const std::vector<ResultResponse>>& response);
    void resultHarmonics(std::variant< const std::vector<ResultHarmonics>>& harmonics);

protected:
    void timerEvent(QTimerEvent * e) override;
    BackendFaust * backend;
    QFaustView * faust_gui;
};

// TODO : factorize this
class QBackendJack : public QObject {
    Q_OBJECT
public:
    QBackendJack(QJackView * gui, QString name);
    ~QBackendJack();
signals:
    void changed();
    void resultResponse(const BackendJack::ResultResponseVar &);
    void resultHarmonics(const BackendJack::ResultHarmonicsVar &);

protected:
    void timerEvent(QTimerEvent * e) override;
    BackendJack * backend;
    QJackView * jack_gui;
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

    QVector<QBackendFaust *> faust;
    QJack * jack;
    MainWindow * mw;
};

