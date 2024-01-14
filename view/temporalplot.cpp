#include "temporalplot.h"


void PlotTemporal::setCurve(const FDF&f){
    auto a = f.frequencyDomainTotemporal();
    QVector<double> qa(a.begin(),a.end());
    QVector<double> qt(a.size());
    for(int i = 0; i < qt.size(); i++){
        qt[i] = (1.0*i) / f.getSampleRate();
        qa[i] /= qt.size();
    }
    values->setData(qt,qa,true);
    values->rescaleAxes(false);
}


TemporalPlot::TemporalPlot(QWidget * parent):QCustomPlot(parent){
    temporalAxis = xAxis;
    amplitudeAxis = yAxis;

    temporalAxis->setLabel("Temps (s)");

    amplitudeAxis->setLabel("Amplitude");

}
TemporalPlot::~TemporalPlot(){
    for(auto i : plots){
        delete i;
    }
}




void TemporalPlot::setPlot(const FDF & f, QString name){
    if(!plots.contains(name)){
        auto pa = addGraph(temporalAxis, amplitudeAxis);

        QPen pen_amplitude;
        auto color = color_round_robin.getNext();
        pen_amplitude.setColor(color);
        pen_amplitude.setWidth(2);

        pen_amplitude.setStyle(Qt::SolidLine);
        pa->setPen(pen_amplitude);

        plots.insert(name,new PlotTemporal(color, pa));
    }
    plots[name]->setCurve(f);
}


void TemporalPlot::updatePlot(QString name, const FDF&v){
    auto * p = plots[name];
    assert(p);
    p->setCurve(v);
}