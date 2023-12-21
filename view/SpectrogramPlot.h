#pragma once
#include <QWidget>


#include <qcustomplot.h>
#include "Spectrogram.h"

class SpectrogramPlot : public QCustomPlot {
    Q_OBJECT

public:
    SpectrogramPlot(QWidget* parent = nullptr);

    void plotSpectrogram(const ResultSpectrogram& spectrogram);
    void setResult(const std::variant<const std::vector<ResultResponse>> & r){
        auto v = get<const std::vector<ResultResponse>>(r);

        auto t = v[0].response.frequencyDomainTotemporal();

        auto spect = spectrogram(t
                                 ,5
                                 ,64
                                 , v[0].response.getSampleRate());
        plotSpectrogram(spect);
    }



private:

};

