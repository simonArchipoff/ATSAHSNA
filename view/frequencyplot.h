#pragma once

#include <QColor>
#include <qcustomplot.h>
#include <FrequencyDomain.h>
#include "roundrobincolor.h"



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
    ~FrequencyPlot();
    void setPlot(const FDF & f, QString name, bool phaseDisp=true);

    void updatePlot(QString name, const FDF&v);
protected:
    QMap<QString,PlotAmplitudePhase *> plots;
    RoundRobinColor color_round_robin;
    QCPAxis *frequencyAxis;
    QCPAxis *amplitudeAxis;
    QCPAxis *phaseAxis;
};
