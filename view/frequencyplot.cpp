#include "frequencyplot.h"




PlotAmplitudePhase::PlotAmplitudePhase(QString name, QColor c,QCPGraph * amplitude, QCPGraph * phase):amplitude(amplitude),phase(phase),color(c),name(name)
{
}

void PlotAmplitudePhase::setCurve(const FDF&f){
    auto fr = f.getFrequency();
    auto fa = f.getAmplitude20log10();
    auto fp = f.getPhase();

    const int n = 1048;

    if(fr.size() < n){
        setCurve(fr,
                 fa,
                 fp);
        return;
    }

    VD frd(n), fad(n),fpd(n);
    const double s = 1.l/n;
    for(uint i = 0; i < n; i++){
        const double d = (exp(i * s) - 1) / (M_E - 1)  ;
        const uint idx = d * fr.size();
        //assert(idx >= 0);
        assert(idx < fr.size());

        frd[i] = fr[idx];
        fad[i] = fa[idx];
        fpd[i] = fp[idx];
    }
    setCurve(frd,
             fad,
             fpd);
}

void PlotAmplitudePhase::setCurve(const VD&f, const VD&a, const VD&p){
    auto qf = QVector<double>(f.begin(), f.end());
    auto qa = QVector<double>(a.begin(), a.end());
    auto qp = QVector<double>(p.begin(), p.end());
    amplitude->setData(qf,qa,true);
    phase->setData(qf,qp,true);
    amplitude->rescaleAxes(false);
}

FrequencyPlot::FrequencyPlot(QWidget * parent):MyQCustomPlot(parent){
    frequencyAxis = xAxis;
    amplitudeAxis = yAxis;
    phaseAxis = yAxis2;
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
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

void FrequencyPlot::setPlot(const FDF & f, QString name, QColor color){
    if(!plots.contains(name)){
        auto pa = addGraph(frequencyAxis, amplitudeAxis);
        auto pp = addGraph(frequencyAxis, phaseAxis);

        QPen pen_phase,pen_amplitude;
        pen_phase.setColor(color);
        pen_phase.setWidth(2);
        pen_amplitude = pen_phase;
        pen_phase.setStyle(Qt::DotLine);
        pen_amplitude.setStyle(Qt::SolidLine);
        phaseAxis->setVisible(display_phase);
        pa->setPen(pen_amplitude);
        pp->setPen(pen_phase);

        plots.insert(name,new PlotAmplitudePhase(name, color,pa,pp));
    }
    plots[name]->setDisplayPhase(display_phase);
    plots[name]->setCurve(f);
    phaseAxis->setVisible(display_phase);
}

void FrequencyPlot::removeResult(QString name){
    auto r = plots[name];
    this->removePlottable(r->amplitude);
    this->removePlottable(r->phase);
    replot();
    plots.remove(name);
    delete r;
}

void FrequencyPlot::updatePlot( const FDF&v, QString name){
    auto * p = plots[name];
    assert(p);
    p->setCurve(v);
}

void FrequencyPlot::displayPhase(bool b){
    display_phase = b;
    for(auto & i : plots){
        i->setDisplayPhase(b);
    }
    replot();
}
