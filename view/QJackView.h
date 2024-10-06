#pragma once
#include "qspinbox.h"
#include <QWidget>
#include <Response.h>
#include <Harmonics.h>
#include <QPushButton>
#include <jack/jack.h>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <jack/types.h>
#include <vumetre.h>
#include <spectrummonitor.h>
#include <qlogging.h>
#include <widget_chatgpt.h>

class QJackPortView : public QWidget{
    Q_OBJECT
public:
    QJackPortView(QWidget *parent);
    ~QJackPortView();
    void setName(QString);
    void setConnexionName(QString);
    void setLevel(float l,float max);

protected:
    QLabel * name;
    QLabel * nameConnexion;
    VUMeter * vumeter;
};

class QJackPortManager : public QWidget {
    Q_OBJECT
public:
    QJackPortManager(QWidget *parent = nullptr);
    ~QJackPortManager();

    void add(jack_port_id_t port, QString portName);
    void remove(jack_port_id_t port);
    void connectPort(jack_port_id_t a, jack_port_id_t b,QString nameb);
    void disconnectPort(jack_port_id_t a, jack_port_id_t b);
    QJackPortView * getPort(jack_port_id_t  port);
    void updateLevels(Levels l);


private:
    QVBoxLayout *layout;
    QMap<jack_port_id_t, QJackPortView *> portMap;
    QMap<QString, jack_port_id_t> portMapName;
};

class QJackView : public QWidget {
    Q_OBJECT
public:
    QJackView(QWidget * parent=nullptr);
    void set_sample_rate(uint);
    void set_buffer_size(uint);
    void addPort(jack_port_id_t port, QString portName);
    void removePort(jack_port_id_t port);
    void connectPort(jack_port_id_t a, jack_port_id_t b, QString namea, QString nameb);
    void disconnectPort(jack_port_id_t a, jack_port_id_t b);
    void updateLevels(Levels l);
    void displayError(ErrorBackend e);
    void shutdown(){
        xruns=0;
        connexion_failed("shutdown");
        bufferSize->setText("");
        sampleRate->setText("");
    }

    void click_connect_button(){
        connectionButton->click();
    }


    void connected();
    void connexion_failed(QString);
    void xrun();

signals:
    void requestNewInputPort(QString);
    void requestNewOutputPort(QString);
    void requestResponse(ParamResponse p, bool continuous, int integration=1);
    void requestHarmonics(ParamHarmonics p, bool continuous);
    void requestConnect(QString name);

protected:
    QPushButton * inputButton, * outputButton,*connectionButton;
    QLabel * sampleRate, *bufferSize,*status,*xruns_label;
    int xruns=0;
    QLineEdit * inputName, *outputName;
    QDoubleSpinBox * gain;
    QJackPortManager * portManager;
    ParamResponseWidget * paramresponsewidget;
};
