#pragma once

#include <BodePlot.h>
#include <temporalplot.h>
#include <SpectrogramPlot.h>

class QDisplays : public QWidget{
public:
    QDisplays(QWidget * parents);

    bool isBodeInit();
    bool isSpectrogramInit();

    void setResult(std::variant<const std::vector<ResultResponse>>&r);

    BodePlot * getBodePlot();
    TemporalPlot * getTemporalPlot();
    SpectrogramPlot * getSpectrogramPlot();
protected:
    BodePlot * bodePlot;
    TemporalPlot * temporalPlot;
    SpectrogramPlot * spectrogramPlot;
};


