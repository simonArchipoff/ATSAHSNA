#pragma once
#include <QWidget>


#include <qcustomplot.h>
#include "Spectrogram.h"

class SpectrogramPlot : public QCustomPlot {
    Q_OBJECT

public:
    SpectrogramPlot(QWidget* parent = nullptr);

    void plotSpectrogram(const ResultSpectrogram& spectrogram);

private:

};

