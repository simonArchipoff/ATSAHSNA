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
    virtual QWidget * getConfigureWidget(){
#pragma warning "this should be removed"
        return new QWidget;
    }

private:
    void popWindow(bool b);
    QVBoxLayout * layout;
};

