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

protected:
    void addWidget(QWidget * w);
    virtual QWidget * getConfigureWidget() = 0;

private:
    void popWindow(bool b);
    QVBoxLayout * layout;
};
