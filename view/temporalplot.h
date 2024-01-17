#pragma once


#include <FrequencyDomain.h>
#include <roundrobincolor.h>
#include <qcustomplot.h>

class PlotTemporal{
public:
    PlotTemporal(QColor color,QCPGraph * values):
        color(color)
        ,values(values){
    }
    void setCurve(const FDF & f);
    QColor color;
    QCPGraph * values;
};

class TemporalPlot : public QCustomPlot
{
    Q_OBJECT
public:
    TemporalPlot(QWidget * parent=nullptr);
    ~TemporalPlot();
    void setPlot(const FDF & f, QString name);
    void updatePlot(QString name, const FDF&v);
protected:
    QMap<QString,PlotTemporal *> plots;
    RoundRobinColor color_round_robin;
    QCPAxis *temporalAxis;
    QCPAxis *amplitudeAxis;
};

