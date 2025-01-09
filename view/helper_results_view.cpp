#include "helper_results_view.h"
#include <QApplication>
#include <QPalette>


MyQCustomPlot::MyQCustomPlot(QWidget *parent)
    : QCustomPlot(parent){

    auto p = QApplication::palette();
    auto backgroundColor = p.color(QPalette::Base);
    auto textColor = p.color(QPalette::Text);
    //auto f = p.color(QPalette::ColorRole::AlternateBase);


    for(auto & i : {xAxis,yAxis,xAxis2,yAxis2}){
        i->setLabelColor(textColor);
        i->setBasePen(QPen(textColor, 2));
        i->setTickPen(QPen(textColor, 2));
        i->setSubTickPen(QPen(textColor, 1));
        i->setTickLabelColor(textColor);
    }

    xAxis->grid()->setPen(QPen(textColor, 1, Qt::DotLine));
    yAxis->grid()->setPen(QPen(textColor, 1, Qt::DotLine));
    xAxis->grid()->setSubGridPen(QPen(textColor.lighter().lighter(), 1, Qt::DotLine));
    yAxis->grid()->setSubGridPen(QPen(textColor.lighter().lighter(), 1, Qt::DotLine));
    xAxis->grid()->setSubGridVisible(true);
    yAxis->grid()->setSubGridVisible(true);
    xAxis->grid()->setZeroLinePen(Qt::NoPen);
    yAxis->grid()->setZeroLinePen(Qt::NoPen);
    xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    /*QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    setBackground(backgroundColor);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
*/
    axisRect()->setBackground(backgroundColor);
    setBackground(backgroundColor);
    rescaleAxes();
}

ResultBase::ResultBase(QString name, QWidget *parent):QWidget(parent),layout(new QVBoxLayout){
    setLayout(layout);
    QHBoxLayout * hlayout = new QHBoxLayout;
    layout->addLayout(hlayout);
    auto * label = new QLabel(name,this);
    hlayout->addWidget(label);
    hlayout->addStretch();
    button = new QPushButton("☰",this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    hlayout->addWidget(button);
    button->setCheckable(true);
    button->setDisabled(true);
    connect(button,&QPushButton::clicked,this,&ResultBase::popWindow);
}

void ResultBase::setConfigureWidget(QWidget *w){
    configureWidget = w;
    button->setDisabled(false);
    //configureWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

void ResultBase::addWidget(QWidget *w){
    layout->addWidget(w);
}

void ResultBase::popWindow(bool b){
    configureWidget->setVisible(b);
    if(b){
        layout->insertWidget(1,configureWidget);
    } else {
        layout->removeWidget(configureWidget);
    }
}

