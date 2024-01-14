#pragma once




#include <QTabWidget>

#include <QColor>
#include <QVector>

#include <qcustomplot.h>

#include <Response.h>
#include <Harmonics.h>

#include <SpectrogramPlot.h>

class RoundRobinColor{
public:
    RoundRobinColor(std::initializer_list<QColor> &l);
    RoundRobinColor();
    QColor getNext();

protected:
    QVector<QColor> color;
    uint i;
};

struct PlotAmplitudePhase{

    PlotAmplitudePhase(QString name, QColor c,QCPGraph * amplitude, QCPGraph * phase);

    void setCurve(const FDF&);
    void setCurve(const VD&f, const VD&a, const VD&p);
    void setDisplayPhase(bool phase){
        phaseDisplayed = phase;
        this->phase->setVisible(phase);
    }

    QCPGraph *amplitude,*phase;

    double maxAmplitude,minAmplitude;
    double maxFrequency,minFrequency;
    bool displayed = true;
    bool phaseDisplayed = true;
    QColor color;
    QString name;
};

class FrequencyPlot : public QCustomPlot
{
    Q_OBJECT
public:
    FrequencyPlot(QWidget * parent=nullptr);

    void setPlot(const FDF & f, QString name,bool phaseDisp=true);

    void updatePlot(QString name, const FDF&v);
protected:
    QMap<QString,PlotAmplitudePhase *> plots;
    RoundRobinColor color_round_robin;
    QCPAxis *frequencyAxis;
    QCPAxis *amplitudeAxis;
    QCPAxis *phaseAxis;
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






