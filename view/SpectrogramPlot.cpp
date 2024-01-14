#include "SpectrogramPlot.h"

SpectrogramPlot::SpectrogramPlot(QWidget* parent)
    : QCustomPlot(parent) {
    // Initialiser QCustomPlot
    addGraph();
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    xAxis->setLabel("Temps (s)"); // L'unité de temps est maintenant en secondes
    yAxis->setLabel("Fréquence");
}

void SpectrogramPlot::plotSpectrogram(const ResultSpectrogram& spectrogram) {
    if (spectrogram.data.empty() || spectrogram.frequencies.empty() ||
        spectrogram.max_idx_time_rank <= 0 || spectrogram.max_freq_rank <= 0 || spectrogram.sampleRate <= 0.0) {
        return;
    }

    double timeStep = 1.0 / spectrogram.sampleRate; // Calculez le pas de temps en secondes


    // Configuration de l'échelle des axes
    //xAxis->setRange(0, spectrogram.max_idx_time_rank * timeStep); // L'axe du temps est maintenant en secondes
    yAxis->setScaleType(QCPAxis::stLogarithmic);

    // Afficher le spectrogramme en 2D
    //addGraph(xAxis,yAxis);
    QCPColorMap* colorMap = new QCPColorMap(xAxis, yAxis);
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
