#include "frequencyplot.h"




PlotAmplitudePhase::PlotAmplitudePhase(QString name, QColor c,QCPGraph * amplitude, QCPGraph * phase):amplitude(amplitude),phase(phase),color(c),name(name)
{
}

void PlotAmplitudePhase::setCurve(const FDF&f){
    setCurve(f.getFrequency(),
             f.getAmplitude20log10(),
             f.getPhase());
}

void PlotAmplitudePhase::setCurve(const VD&f, const VD&a, const VD&p){
    auto qf = QVector<double>(f.begin(), f.end());
    auto qa = QVector<double>(a.begin(), a.end());
    auto qp = QVector<double>(p.begin(), p.end());
    amplitude->setData(qf,qa,true);
    phase->setData(qf,qp,true);
    amplitude->rescaleAxes(false);
}

FrequencyPlot::FrequencyPlot(QWidget * parent):QCustomPlot(parent){
    frequencyAxis = xAxis;
    amplitudeAxis = yAxis;
    phaseAxis = yAxis2;

    frequencyAxis->setScaleType(QCPAxis::stLogarithmic);
    frequencyAxis->setLabel("Frequency (Hz)");

    amplitudeAxis->setLabel("Amplitude");

    phaseAxis->setLabel("Phase (degree)");
    phaseAxis->setRange(-180,180);
}
FrequencyPlot::~FrequencyPlot(){
    for(auto i : plots){
        delete i;
    }
}

void FrequencyPlot::setPlot(const FDF & f, QString name, bool phaseDisp){
    if(!plots.contains(name)){
        auto pa = addGraph(frequencyAxis, amplitudeAxis);
        auto pp = addGraph(frequencyAxis, phaseAxis);

        QPen pen_phase,pen_amplitude;
        auto color = color_round_robin.getNext();
        pen_phase.setColor(color);
        pen_phase.setWidth(2);
        pen_amplitude = pen_phase;
        pen_phase.setStyle(Qt::DotLine);
        pen_amplitude.setStyle(Qt::SolidLine);
        phaseAxis->setVisible(phaseDisp);
        pa->setPen(pen_amplitude);
        pp->setPen(pen_phase);

        plots.insert(name,new PlotAmplitudePhase(name, color,pa,pp));
    }
    plots[name]->setDisplayPhase(phaseDisp);
    plots[name]->setCurve(f);
}


void FrequencyPlot::updatePlot(QString name, const FDF&v){
    auto * p = plots[name];
    assert(p);
    p->setCurve(v);
}


