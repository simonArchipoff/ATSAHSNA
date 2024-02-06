#pragma once


#include <FrequencyDomain.h>
#include <roundrobincolor.h>
#include <helper_results_view.h>
#include <Response.h>

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

class TemporalPlot : public MyQCustomPlot
{
    Q_OBJECT
public:
    TemporalPlot(QWidget * parent=nullptr);
    ~TemporalPlot();
    void setPlot(const FDF & f, QString name, QColor color);
    void updatePlot( const FDF&v, QString name);
    void updatePlot(const ResultResponse&v, QString name){
        updatePlot(v.response, name);
    }
    void removeResult(QString);
protected:
    QMap<QString,PlotTemporal *> plots;
    RoundRobinColor color_round_robin;
    QCPAxis *temporalAxis;
    QCPAxis *amplitudeAxis;
};

