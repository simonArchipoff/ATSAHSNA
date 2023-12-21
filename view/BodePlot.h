#pragma once


#include "SpectrogramPlot.h"
#include <QChartView>
#include <QLineSeries>
#include <QLogValueAxis>
#include <QValueAxis>

#include<QTabWidget>

#include <QColor>

#include <Response.h>
#include <Harmonics.h>



class RoundRobinColor{
public:
    RoundRobinColor(std::initializer_list<QColor> &l);
    RoundRobinColor();
    QColor getNext();

protected:
    QVector<QColor> color;
    uint i;
};

class PlotAmplitudePhase:public QObject{
    Q_OBJECT
public:
    PlotAmplitudePhase(QString name, QColor c);

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
    FrequencyPlot(QWidget * parent=nullptr);

    void addPlot(const FDF & f, QString name, bool phaseDisp = true);

    void updatePlot(QString name, const FDF&v);

protected:
    QMap<QString,PlotAmplitudePhase *> plots;
    QChart chart;
    QValueAxis * amplitude_axis,*phase_axis;
    QLogValueAxis * frequency_axis;
    RoundRobinColor color_round_robin;
};


class BodePlot : public FrequencyPlot
{
    Q_OBJECT
public:
    BodePlot(QWidget*parent);
    void setResult(std::variant<const std::vector<ResultResponse>> & r);
};

class THDPlot : public FrequencyPlot
{
public:
    THDPlot(QWidget* parent);
    void setResult(std::variant<const std::vector<ResultHarmonics>> & r);
};


class QDisplays : public QTabWidget{
public:
    QDisplays(QWidget * parents);

    bool isBodeInit();
    bool isTHDinit();
    bool isSpectrogramInit();

    BodePlot * getBodePlot();
    THDPlot * getTHDPlot();
    SpectrogramPlot * getSpectrogramPlot();
protected:
    BodePlot * bodePlot;
    THDPlot * thdPlot;
    SpectrogramPlot * spectrogramPlot;
};






