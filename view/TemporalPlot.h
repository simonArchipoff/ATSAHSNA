#pragma once
#include <QWidget>

#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>

#include "Response.h"
#include "qtmetamacros.h"

#include <qcustomplot.h>



class TemporalSignal : public QChartView{
    Q_OBJECT
public:
    TemporalSignal();
    ~TemporalSignal();
    void setCurve(const VD&s, int sampleRate, QString, QColor c);
    void updateCurve(const VD&s, QString);

private:
    QMap<QString,QLineSeries*> signal_series;
    QMap<QString,int> sampleRates;
    QChart chart;
    QValueAxis *amplitude_axis,*temporal_axis;
    double tmin,tmax;
    double ymax;
};


