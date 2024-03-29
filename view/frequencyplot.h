#pragma once

#include <QColor>
#include <qcustomplot.h>
#include <FrequencyDomain.h>
#include <Response.h>
#include <helper_results_view.h>


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

class FrequencyPlot : public MyQCustomPlot
{
    Q_OBJECT
public:
    FrequencyPlot(QWidget * parent=nullptr);
    ~FrequencyPlot();
    void setPlot(const FDF & f, QString name, QColor c);
    void removeResult(QString name);
    void updatePlot(const FDF&v,QString name);
    void updatePlot(const ResultResponse & r, QString name){
        updatePlot(r.response, name);
    }
    void displayPhase(bool b);
protected:
    QMap<QString,PlotAmplitudePhase *> plots;
    QCPAxis *frequencyAxis;
    QCPAxis *amplitudeAxis;
    QCPAxis *phaseAxis;
    bool display_phase = true;
};
