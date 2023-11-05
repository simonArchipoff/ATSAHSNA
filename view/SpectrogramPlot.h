#pragma once
#include <QWidget>

#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>


#include <qcustomplot.h>>
#include "Spectrogram.h"

class SpectrogramPlot : public QCustomPlot {
    Q_OBJECT

public:
    SpectrogramPlot(QWidget* parent = nullptr);

    void plotSpectrogram(const ResultSpectrogram& spectrogram);

private:
         // Ajoutez des membres de données privées ou des méthodes privées au besoin
};

