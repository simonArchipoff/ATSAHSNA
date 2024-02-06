#include "SpectrogramPlot.h"

SpectrogramPlot::SpectrogramPlot(QWidget* parent)
    : MyQCustomPlot(parent) {
    // Initialiser QCustomPlot
    addGraph();
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    yAxis->setScaleType(QCPAxis::stLogarithmic);
    colorMap = new QCPColorMap(xAxis, yAxis);
    xAxis->setLabel(tr("Time (s)")); // L'unité de temps est maintenant en secondes
    yAxis->setLabel(tr("Frequency"));
}
SpectrogramPlot::~SpectrogramPlot(){
    //delete colorMap;
}

void SpectrogramPlot::plotSpectrogram(const ResultSpectrogram& spectrogram) {
    if (spectrogram.data.empty() || spectrogram.frequencies.empty() ||
        spectrogram.max_idx_time_rank <= 0 || spectrogram.max_freq_rank <= 0 || spectrogram.sampleRate <= 0.0) {
        return;
    }

    double timeStep = 1.0 / spectrogram.sampleRate; // Calculez le pas de temps en secondes
    colorMap->data()->clear();
    colorMap->data()->setSize(spectrogram.max_idx_time_rank, spectrogram.max_freq_rank);
    colorMap->data()->setRange(QCPRange(0, spectrogram.max_idx_time_rank * timeStep), QCPRange(*spectrogram.frequencies.begin()
                                                                                               ,*spectrogram.frequencies.rbegin()));
    for (uint i = 0; i < spectrogram.data.size(); ++i) {
        colorMap->data()->setCell(i % spectrogram.max_idx_time_rank, i / spectrogram.max_idx_time_rank, spectrogram.data[i]);
    }
    colorMap->rescaleDataRange();
    colorMap->setGradient(QCPColorGradient::gpJet);
    colorMap->setInterpolate(false);

    rescaleAxes();
    replot();
}

void SpectrogramPlot::setResult(const ResultResponse &r){
    auto t = r.response.frequencyDomainTotemporal();
#if 0
        auto spect = stft(t.data(), t.data() + t.size(), 1024, 2, v[0].response.getSampleRate());
#else

    auto spect = spectrogram(t
                             ,6
                             ,128
                             ,r.response.getSampleRate());
#endif
    plotSpectrogram(spect);
}


/*

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

*/

SpectrogramPlots::SpectrogramPlots(QWidget *parent):ResultBase("spectrogram",parent),m(),layout(new QHBoxLayout){
    auto w = new QWidget(this);
    w->setLayout(layout);
    addWidget(w);
}

void SpectrogramPlots::setResult(const ResultResponse& r, QString name){
    if(!m.contains(name)){
        auto s = new SpectrogramPlot(this);
        m.insert(name,s);
        layout->addWidget(s);
    }
    if(isVisible())
        m[name]->setResult(r);
}

void SpectrogramPlots::removeResult(QString name){
    auto r = m[name];
    m.remove(name);
    delete r;
}
