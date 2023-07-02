#pragma once

#include "qchart.h"
#include "qlineseries.h"
#include "qnamespace.h"
#include <QMap>
#include <QWidget>
#include <QSlider>
#include <QChart>
#include <QList>
#include <QLegendMarker>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include <vector>
#include <set>

#include <signalResponse.h>
#include <signalHarmonics.h>

class RoundRobinColor{
public:
    RoundRobinColor(std::initializer_list<QColor> &l):color(l),i(0){}
    RoundRobinColor():i(0){
        using namespace Qt;
        for(auto & i : {green,
                 red,
                 blue,
                 cyan,
                 magenta,
                 yellow,
                 black,
                 darkGray,
                 gray,
                 darkRed,
                 darkGreen,
                 darkBlue,
                 darkCyan,
                 darkMagenta,
                 darkYellow}){ //QColor::colorNames()){
            color.push_back(QColor(i));
        }
    }
    QColor getNext(){
        i  %= color.size();
        return color[i++];
    }
    QVector<QColor> color;
    uint i;
};

class PlotAmplitudePhase:public QObject{
    Q_OBJECT
public:
    PlotAmplitudePhase(QString name, QColor c):amplitude(new QLineSeries),phase(new QLineSeries),color(c),name(name)
    {
        QPen p = amplitude->pen();
        p.setColor(c);
        p.setStyle(Qt::PenStyle::SolidLine);
        p.setWidth(2);
        amplitude->setPen(p);
        p.setStyle(Qt::PenStyle::DotLine);
        phase->setPen(p);

    }
    void setCurve(const VD&f, const VD&a, const VD&p, QString name);

    QScopedPointer<QLineSeries> amplitude,phase;
    double maxAmplitude,minAmplitude;
    double maxFrequency,minFrequency;
    bool displayed = true;
    bool phaseDisplayed = true;
    QColor color;
    QString name;
};
class FrequencyPlot : public QChartView
{
    Q_OBJECT
public:
    FrequencyPlot(QWidget * parent=nullptr):QChartView(parent)
                                    ,amplitude_axis(new QValueAxis)
                                    ,phase_axis(new QValueAxis)
                                    ,frequency_axis(new QLogValueAxis){
        chart.addAxis(frequency_axis, Qt::AlignBottom);
        chart.addAxis(amplitude_axis, Qt::AlignLeft);
        chart.addAxis(phase_axis,Qt::AlignRight);
        frequency_axis->setBase(10);

        setChart(&chart);
    }

    void addPlot(const FDF & f, QString name){
        if(plots.count(name) == 0) {
            auto c = color_round_robin.getNext();
            PlotAmplitudePhase * p = new PlotAmplitudePhase(name,c);
            plots.insert(name,p);
            chart.addSeries(p->amplitude.data());

            p->amplitude->attachAxis(amplitude_axis);
            p->amplitude->attachAxis(frequency_axis);

            chart.addSeries(p->phase.data());
            p->phase->attachAxis(phase_axis);
            p->phase->attachAxis(frequency_axis);
            chart.legend()->markers(p->phase.data())[0]->setVisible(false);
        }
        updatePlot(name,f);
    }

    void updatePlot(QString name, const FDF&v){
        auto * p = plots[name];
        assert(p);
        p->setCurve(v.getFrequency(),v.getAmplitude20log10(),v.getPhase(),name);
    }

    QMap<QString,PlotAmplitudePhase *> plots;
    QChart chart;
    QValueAxis * amplitude_axis,*phase_axis;
    QLogValueAxis * frequency_axis;
    RoundRobinColor color_round_robin;
};

/*
    void setResponse(const ResultResponse & r){
        auto a = r.response.getAmplitude20log10();
        auto p = r.response.getPhase();
        auto f = r.response.getFrequency();
        transform(amplitude,f,a);
        transform(phase,f,p);
    }*/

/*
void setResponses(std::variant<const std::vector<ResultResponse>> & r){
    std::vector<ResultResponse> v = get<const std::vector<ResultResponse>>(r);
    amplitude.data()->clear();
    phase.data()->clear();

    plots->setResponse(v[0]);
    phase_axis->setRange(-180,180);
    amplitude_axis->setRange(-100,20);
    frequency_axis->setRange(10,30000);

    chart.update();
}
*/



class BodePlot : public FrequencyPlot
{
    Q_OBJECT
public:
    BodePlot(QWidget*parent):FrequencyPlot(parent){}
    void setResponses(std::variant<const std::vector<ResultResponse>> & r){
        std::vector<ResultResponse> v = get<const std::vector<ResultResponse>>(r);
        for(uint i = 0; i < v.size(); i++){
            addPlot(v[i].response,QString{v[i].name.data()}+ "_" + QString::number(i));
        }
        phase_axis->setRange(-180,180);
        amplitude_axis->setRange(-100,20);
        frequency_axis->setRange(20,30000);
        chart.update();
    }

};

class THDPlot : public FrequencyPlot
{
public:
    THDPlot(QWidget* parent);
    void setResult(const ResultHarmonics &, QColor c);
    //void setThdResult
};
