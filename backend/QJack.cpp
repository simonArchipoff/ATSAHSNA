#include "QJack.h"
#include <QTimer>

QJack::QJack(QObject *parent):QObject(parent){
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &QJack::updateLevel);
    timer->start(100);
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


