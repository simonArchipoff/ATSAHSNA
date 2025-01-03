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


    void setConfigureWidget(QWidget * w);


protected:
    void addWidget(QWidget * w);
    QPushButton * button;
private:
    void popWindow(bool b);
    QVBoxLayout * layout;
    QWidget * configureWidget;
};
