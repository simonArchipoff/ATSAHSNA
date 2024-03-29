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


TemporalPlot::TemporalPlot(QWidget * parent):ResultBase(tr("temporal plot"),parent),plot(new MyQCustomPlot(parent)){
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    temporalAxis = plot->xAxis;
    amplitudeAxis = plot->yAxis;
    temporalAxis->setLabel(tr("time (s)"));
    amplitudeAxis->setLabel(tr("Amplitude"));
    addWidget(plot);
}
TemporalPlot::~TemporalPlot(){
    for(auto i : plots){
        delete i;
    }
}




void TemporalPlot::setPlot(const FDF & f, QString name, QColor color){
    if(!plots.contains(name)){
        auto pa = plot->addGraph(temporalAxis, amplitudeAxis);

        QPen pen_amplitude;

        pen_amplitude.setColor(color);
        pen_amplitude.setWidth(2);

        pen_amplitude.setStyle(Qt::SolidLine);
        pa->setPen(pen_amplitude);

        plots.insert(name,new PlotTemporal(color, pa));
    }
    plots[name]->setCurve(f);
}


void TemporalPlot::updatePlot(const FDF&v , QString name){
    auto * p = plots[name];
    assert(p);
    p->setCurve(v);
}

void TemporalPlot::removeResult(QString name){
    auto r = plots[name];
    plots.remove(name);
    if(r){
        plot->removePlottable(r->values);
        plot->replot();
        delete r;
    }
}
