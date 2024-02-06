#pragma once

#include <BodePlot.h>
#include <temporalplot.h>
#include <SpectrogramPlot.h>

#include <qmeasuresview.h>

class QResponseView : public QWidget{
    Q_OBJECT
public:
    QResponseView(QWidget * parents);

    bool isBodeInit();
    bool isSpectrogramInit();

    void setResults(std::variant<const std::vector<ResultResponse>> & r);
    void setResult(const ResultResponse &r);
    void removeResult(QString);

    void setBodeView(bool b){
        bbodePlot->setVisible(b);
    }
    void setTemporalView(bool b){
        btemporalPlot->setVisible(b);
    }
    void setSpectrogramView(bool b){
        bspectrogramPlot->setVisible(b);
    }

    BodePlot * getBodePlot();
    TemporalPlot * getTemporalPlot();
    SpectrogramPlots * getSpectrogramPlot();
protected:
    BodePlot * bodePlot;
    TemporalPlot * temporalPlot;
    SpectrogramPlots * spectrogramPlot;

    QWidget * bbodePlot,
     * btemporalPlot,
     * bspectrogramPlot;
    QMeasuresView * measures;
};


