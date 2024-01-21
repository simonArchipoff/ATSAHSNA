#pragma once

#include <BodePlot.h>
#include <temporalplot.h>
#include <SpectrogramPlot.h>

class QResponseView : public QWidget{
public:
    QResponseView(QWidget * parents);

    bool isBodeInit();
    bool isSpectrogramInit();

    void setResult(std::variant<const std::vector<ResultResponse>>&r);

    BodePlot * getBodePlot();
    TemporalPlot * getTemporalPlot();
    SpectrogramPlots * getSpectrogramPlot();
protected:
    BodePlot * bodePlot;
    TemporalPlot * temporalPlot;
    SpectrogramPlots * spectrogramPlot;
};


