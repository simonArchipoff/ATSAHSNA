#pragma once

#include "Jack.h"
#include <QObject>




class QJack :public QObject, public BackendJack {
    Q_OBJECT
public:
    QJack(QObject*parent);

    void startResponse(ParamResponse p, bool continuous, int integration);

signals:
    void jack_request_failed(ErrorBackend);
    void jack_started_s();
    void jack_started_failed_s(QString);
    void jack_shutdown_s();
    void jack_info_shutdown_s(jack_status_t code, const char *reason);
    int jack_buffer_size_s(jack_nframes_t nframes);
    int jack_samplerate_s(jack_nframes_t nframes);
    void jack_client_registration_s(QString name, int i);
    void jack_port_registration_s(jack_port_id_t port, int i,QString name);
    void jack_port_rename_s(jack_port_id_t port,  QString old_name, QString new_name);
    void jack_port_connect_s(jack_port_id_t a, jack_port_id_t b, int connect, QString porta, QString portb);
    //int 	jack_set_port_connect_callback (jack_client_t *, JackPortConnectCallback connect_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    void jack_xrun_s();

    void levels(Levels l);

protected:
    virtual void jack_started()override{
        emit jack_started_s();
    }
    virtual void jack_started_failed(JackStatus s) override {
        QString result;
        if (s & JackFailure)         result += "JackFailure ";
        if (s & JackInvalidOption)   result += "JackInvalidOption ";
        if (s & JackNameNotUnique)   result += "JackNameNotUnique ";
        if (s & JackServerStarted)   result += "JackServerStarted ";
        if (s & JackServerFailed)    result += "JackServerFailed ";
        if (s & JackServerError)     result += "JackServerError ";
        if (s & JackNoSuchClient)    result += "JackNoSuchClient ";
        if (s & JackLoadFailure)     result += "JackLoadFailure ";
        if (s & JackInitFailure)     result += "JackInitFailure ";
        if (s & JackShmFailure)      result += "JackShmFailure ";
        if (s & JackVersionError)    result += "JackVersionError ";
        if (s & JackBackendError)    result += "JackBackendError ";
        if (s & JackClientZombie)    result += "JackClientZombie ";
        emit jack_started_failed_s(result);
    }
    void updateLevel();
    virtual void jack_shutdown()override{
        emit jack_shutdown_s();
        ready = false;
    }
    virtual void jack_info_shutdown(jack_status_t code, const char *reason) override {
        emit jack_info_shutdown_s(code, reason);
        BackendJack::jack_info_shutdown(code,reason);
    }
    virtual int jack_buffer_size(jack_nframes_t nframes)override{
        emit jack_buffer_size_s(nframes);
        return BackendJack::jack_buffer_size(nframes);
    }
    virtual int jack_samplerate(jack_nframes_t nframes)override{
        emit jack_samplerate_s(nframes);
        return BackendJack::jack_samplerate(nframes);
    }
    virtual void jack_client_registration (const char *name, int i)override{
        emit jack_client_registration_s(QString(name),i);
        BackendJack::jack_client_registration(name,i);
    }
    virtual void jack_port_registration(jack_port_id_t port, int i,std::string port_name)override{
        jack_port_t *p = jack_port_by_id(client, port);
        if(p && 0==strncmp(jack_port_name(p),APPNAME,strlen(APPNAME))){
            emit jack_port_registration_s(port,i,QString(jack_port_short_name(p)));
            BackendJack::jack_port_registration(port,i,port_name);
        }
    }
    virtual void jack_port_rename(jack_port_id_t port, const char *old_name, const char *new_name)override{
        BackendJack::jack_port_rename(port,old_name,new_name);
    }
    virtual void jack_port_connect(jack_port_id_t a, jack_port_id_t b, int connect)override{
        auto na  = jack_port_name(jack_port_by_id(client,a));
        auto nb  = jack_port_name(jack_port_by_id(client,b));
        auto nsa = jack_port_short_name(jack_port_by_id(client,a));
        auto nsb = jack_port_short_name(jack_port_by_id(client,b));
        if(0==strncmp(na,APPNAME,strlen(APPNAME))) {
            emit jack_port_connect_s(a,b,connect, QString(nsa),QString(nb));
            BackendJack::jack_port_connect(a,b,connect);
        }

        if(0==strncmp(nb,APPNAME,strlen(APPNAME))) {
            emit jack_port_connect_s(b,a,connect, QString(nsb),QString(na));
            BackendJack::jack_port_connect(b,a,connect);
        }
    }
    //int 	jack_set_port_connect_callback (jack_client_t *, JackPortConnectCallback connect_callback, void *arg) JACK_OPTIONAL_WEAK_EXPORT
    virtual int jack_xrun()override{
        BackendJack::jack_xrun();
        return 0;
    }
};

