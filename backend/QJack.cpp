#include "QJack.h"
#include <QTimer>

QJack::QJack(QObject *parent):QObject(parent){
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &QJack::updateLevel);
    timer->start(100);
}

void QJack::startResponse(ParamResponse p, bool continuous, int integration){
    auto r = BackendJack::startResponse(p,continuous,integration);
    ErrorBackend * e = std::get_if<ErrorBackend>(&r);
    if(e){
        emit jack_request_failed(*e);
    }
}

void QJack::updateLevel(){
    Levels l;
    bool res = false;
    while(getLevels(l)){
        res=true;
    }
    if(res){
        emit levels(l);
    }
}


