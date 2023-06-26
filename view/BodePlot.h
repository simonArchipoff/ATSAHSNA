#pragma once

#include "qlineseries.h"
#include <QMap>
#include <QWidget>
#include <QSlider>
#include <QChart>
#include <QLineSeries>
#include <vector>
#include <set>

#include <signalResponse.h>
#include <signalHarmonics.h>



QSharedPointer<QLineSeries>  transform(VD x,VD y);
class Plot:public QObject{
    Q_OBJECT
public:


    void setResponse(const ResultResponse & r){
        auto a = r.response.getAmplitude20log10();
        auto p = r.response.getPhase();
        auto f = r.response.getFrequency();
        amplitude = transform(f,a);
        phase = transform(f,p);
    }

    QSharedPointer<QLineSeries> amplitude,phase;
    QString name;
};

class FrequencyPlot : public QChart
{
    Q_OBJECT
public:
    FrequencyPlot(QWidget * parent){
        plots = new Plot;
    }
    void setResponses(std::variant<const std::vector<ResultResponse>> & r){
        std::vector<ResultResponse> v = get<const std::vector<ResultResponse > >(r);

        plots->setResponse(v[0]);
        update(plots);
    }
    void update(Plot * p){
        plots = p;
        addSeries(plots->amplitude.data());
        addSeries(plots->phase.data());
    }

    Plot * plots;
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
