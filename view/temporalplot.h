#pragma once


#include <FrequencyDomain.h>
#include <roundrobincolor.h>
#include <helper_results_view.h>
#include <Response.h>

class PlotTemporal{
public:
    enum ResponseType {Impulse,Step};
    PlotTemporal(QColor color,QCPGraph * values, ResponseType type):
        color(color)
        ,values(values){
    }
    void setCurve(const FDF & f);
    void plotData();
    QColor color;
    QCPGraph * values;
    ResponseType type;
    FDF response;

};

class TemporalPlotMenu;
class TemporalPlot : public ResultBase{
    Q_OBJECT
public:

    TemporalPlot(QWidget * parent=nullptr);
    ~TemporalPlot();
    void setPlot(const FDF & f, QString name, QColor color);
    void updatePlot( const FDF&v, QString name);
    void updatePlot(const ResultResponse&v, QString name){
        updatePlot(v.response, name);
    }
    void replot(){
        plot->replot();
    }
    void removeResult(QString);
protected:
    QMap<QString,PlotTemporal *> plots;
    RoundRobinColor color_round_robin;
    QCPAxis *temporalAxis;
    QCPAxis *amplitudeAxis;
private:
    MyQCustomPlot * plot;
    TemporalPlotMenu * menu;
private:
    void changeResponseType(PlotTemporal::ResponseType t);

};


class TemporalPlotMenu : public QWidget{
    Q_OBJECT
public:
    TemporalPlotMenu();
signals:
    void responseTypeChanged(enum PlotTemporal::ResponseType);
public:
    PlotTemporal::ResponseType responsetype = PlotTemporal::Impulse;
};
