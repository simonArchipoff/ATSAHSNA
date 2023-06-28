#pragma once

#include "qlineseries.h"
#include "qnamespace.h"
#include <QMap>
#include <QWidget>
#include <QSlider>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include <vector>
#include <set>

#include <signalResponse.h>
#include <signalHarmonics.h>



void  transform(QSharedPointer<QLineSeries> & s, VD x,VD y);

class Plot:public QObject{
    Q_OBJECT
public:
    Plot():amplitude(new QLineSeries),phase(new QLineSeries)
    {
    }


    void setResponse(const ResultResponse & r){
        auto a = r.response.getAmplitude20log10();
        auto p = r.response.getPhase();
        auto f = r.response.getFrequency();
        transform(amplitude,f,a);
        transform(phase,f,p);
    }

    QSharedPointer<QLineSeries> amplitude,phase;
    QList<QPointF> qamplitude,qphase;
    QString name;
};

class FrequencyPlot : public QChartView
{
    Q_OBJECT
public:
    FrequencyPlot(QWidget * parent):QChartView(parent),plots(new Plot),amplitude_axis(new QValueAxis),phase_axis(new QValueAxis),frequency_axis(new QLogValueAxis){
        chart.addAxis(frequency_axis, Qt::AlignBottom);
        chart.addAxis(amplitude_axis, Qt::AlignLeft);
        chart.addAxis(phase_axis,Qt::AlignRight);
        frequency_axis->setBase(10);
        chart.addSeries(plots->amplitude.data());



        plots->amplitude->attachAxis(amplitude_axis);
        plots->amplitude->attachAxis(frequency_axis);


        chart.addSeries(plots->phase.data());
        plots->phase->attachAxis(phase_axis);
        plots->phase->attachAxis(frequency_axis);
        setChart(&chart);
    }
    void setResponses(std::variant<const std::vector<ResultResponse>> & r){
        std::vector<ResultResponse> v = get<const std::vector<ResultResponse > >(r);
        plots->amplitude.data()->clear();
        plots->phase.data()->clear();

        plots->setResponse(v[0]);
        phase_axis->setRange(-180,180);
        amplitude_axis->setRange(-100,20);
        frequency_axis->setRange(10,20000);

        chart.update();
    }
    Plot * plots;
    QChart chart;
    QValueAxis * amplitude_axis,*phase_axis;
    QLogValueAxis * frequency_axis;
};

/*
class ColorMapViridis : public QwtLinearColorMap{
public:
    ColorMapViridis();
};

class ColorMapBlackBody : public QwtLinearColorMap{
public:
    ColorMapBlackBody();
};
*/


class BodeCurve{
public:
  //BodeCurve(const BodeCurve&) = default;
  BodeCurve(const QColor);
  void setCurve(const FDF&,double minFrequency, double maxFrequency);

  QColor color;
  double maxAmplitude,minAmplitude;
  double maxFrequency,minFrequency;
  bool displayed;
};

class BodePlot : public FrequencyPlot
{
  Q_OBJECT
public:
  BodePlot(QWidget * parent);
  void setResult(const FDF &, QColor c,double freqMin, double freqMax);
  void removeCurve(QColor c);
  //QString represent color, because they are orderer, qcolor are not, maybe I could just take a name…
  QMap<QString,BodeCurve *> curves;

  double maxAmplitude = 10, minAmplitude=-100;
};

class THDPlot : public FrequencyPlot
{
public:
  THDPlot(QWidget* parent);
  void setResult(const ResultHarmonics &, QColor c);
  //void setThdResult
};
