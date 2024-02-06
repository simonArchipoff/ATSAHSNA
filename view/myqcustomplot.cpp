#include "myqcustomplot.h"
#include <QApplication>
#include <QPalette>


MyQCustomPlot::MyQCustomPlot(QWidget *parent)
    : QCustomPlot(parent){

    auto p = QApplication::palette("QWidget");
    auto backgroundColor = p.color(QPalette::ColorRole::Base);
    auto textColor = p.color(QPalette::ColorRole::Text);
    auto f = p.color(QPalette::ColorRole::AlternateBase);


    for(auto & i : {xAxis,yAxis,xAxis2,yAxis2}){
        i->setLabelColor(textColor);
        i->setBasePen(QPen(textColor, 1));
        i->setTickPen(QPen(textColor, 1));
        i->setSubTickPen(QPen(textColor, 1));
        i->setTickLabelColor(textColor);
    }

    xAxis->grid()->setPen(QPen(textColor.lighter(), 1, Qt::DotLine));
    yAxis->grid()->setPen(QPen(textColor.lighter(), 1, Qt::DotLine));
    xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
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
    axisRect()->setBackground(backgroundColor);
    */
    rescaleAxes();
}
