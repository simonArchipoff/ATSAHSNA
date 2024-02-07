#pragma once

#include <qcustomplot.h>


class MyQCustomPlot : public QCustomPlot {
    Q_OBJECT
public:
    MyQCustomPlot(QWidget * parent=nullptr);
};

class ResultBase : public QWidget{
    Q_OBJECT
public:
    ResultBase(QString name, QWidget * parent);


    void setConfigureWidget(QWidget * w){
        configureWidget = w;
        //configureWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    }


protected:
    void addWidget(QWidget * w);

private:
    void popWindow(bool b);
    QVBoxLayout * layout;
    QWidget * configureWidget;
};

