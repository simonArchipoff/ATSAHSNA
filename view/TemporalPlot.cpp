#include "TemporalPlot.h"




TemporalSignal::TemporalSignal()
    :amplitude_axis(new QValueAxis),
    temporal_axis(new QValueAxis),
    tmin(0),
    tmax(1),
    ymax(1)
{
    chart.addAxis(amplitude_axis, Qt::AlignLeft);
    chart.addAxis(temporal_axis,Qt::AlignBottom);
    temporal_axis->setRange(tmin,tmax);
    amplitude_axis->setRange(-1,1);
    setChart(&chart);
}

TemporalSignal::~TemporalSignal(){
    for(auto & i:signal_series){
        delete i;
    }
}

void TemporalSignal::setCurve(const VD& signal, int sampleRate, QString name, QColor c){
    auto serie = new QLineSeries;
    auto p =  serie->pen();
    p.setColor(c);
    serie->setPen(p);
    signal_series.insert(name, serie);
    sampleRates.insert(name,sampleRate);
    updateCurve(signal,name);
    chart.addSeries(signal_series[name]);
}

void TemporalSignal::updateCurve(const VD&signal, QString name){
    auto sampleRate = sampleRates[name];
    QList<QPointF> l;

    const float f = 1.0/sampleRate;
    for(uint i = 0; i < signal.size(); i++){
        l.push_back(QPointF(i*f,signal[i]));
    }
    signal_series[name]->replace(l);
}
