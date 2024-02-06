#pragma once
#include <QWidget>
#include <helper_results_view.h>
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



class SpectrogramPlots : public ResultBase{
    Q_OBJECT

public:
    SpectrogramPlots(QWidget* parent = nullptr);
    virtual QWidget * getConfigureWidget() override{
        return new QWidget(this);
    }
    void setResult(const ResultResponse & r, QString name);
    void removeResult(QString);
private:
    QMap<QString, SpectrogramPlot *> m;
    QHBoxLayout * layout;
};
