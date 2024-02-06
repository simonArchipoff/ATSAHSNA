#pragma once
#include <QWidget>
#include <myqcustomplot.h>
#include "Spectrogram.h"
#include <Response.h>
#include <variant>
#include <mutex>

#include <QHBoxLayout>

class SpectrogramPlot : public MyQCustomPlot {
    Q_OBJECT
public:
    SpectrogramPlot(QWidget* parent = nullptr);
    ~SpectrogramPlot();

    void plotSpectrogram(const ResultSpectrogram& spectrogram);
    void setResult(const ResultResponse & r);

private:
    QCPColorMap * colorMap;
};



class SpectrogramPlots : public QWidget{
    Q_OBJECT

public:
    SpectrogramPlots(QWidget* parent = nullptr);

    void setResult(const ResultResponse & r, QString name);
    void removeResult(QString);
private:
    QMap<QString, SpectrogramPlot *> m;
    QHBoxLayout * layout;
};
