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



class QJackPortView : public QWidget{
    Q_OBJECT
public:
    QJackPortView(QWidget *parent);

    void setName(QString);
    void setConnexionName(QString);


protected:
    QLabel * name;
    QLabel * nameConnexion;
};

class QJackPortManager : public QWidget {
    Q_OBJECT
public:
    QJackPortManager(QWidget *parent = nullptr);
    ~QJackPortManager();

    void add(jack_port_id_t port, QString portName);
    void remove(jack_port_id_t port);
    void connectPort(jack_port_id_t a, jack_port_id_t b);
    void disconnectPort(jack_port_id_t a, jack_port_id_t b);
    QJackPortView * getPort(jack_port_id_t  port);

private:
    QVBoxLayout *layout;
    QMap<jack_port_id_t , QJackPortView *> portMap;

};

class QJackView : public QWidget {
    Q_OBJECT
public:
    QJackView(QWidget * parent=nullptr);
    void set_sample_rate(uint);
    void set_buffer_size(uint);
    void addPort(jack_port_id_t port, QString portName);
    void removePort(jack_port_id_t port);
    void connectPort(jack_port_id_t a, jack_port_id_t b);
    void disconnectPort(jack_port_id_t a, jack_port_id_t b);


signals:
    void requestNewInputPort(QString);
    void requestNewOutputPort(QString);

    void requestResponse(ParamResponse p, bool continuous, int integration=1);
    void requestHarmonics(ParamHarmonics p, bool continuous);


protected:

    QPushButton * inputButton, * outputButton;
    QLabel * sampleRate, *bufferSize;
    QLineEdit * inputName, *outputName;
    QDoubleSpinBox * gain;
    QJackPortManager * portManager;
};
