#pragma once




#include <QTabWidget>
#include <frequencyplot.h>

#include <QColor>
#include <QVector>

#include <qcustomplot.h>

#include <Response.h>
#include <Harmonics.h>

#include <SpectrogramPlot.h>

#include "harmonicsplot.h"




class BodePlot : public FrequencyPlot
{
    Q_OBJECT
public:
    BodePlot(QWidget*parent);
    void setResult(std::variant<const std::vector<ResultResponse>> & r);
};



class QDisplays : public QTabWidget{
public:
    QDisplays(QWidget * parents);

    bool isBodeInit();
    bool isTHDinit();
    bool isSpectrogramInit();

    BodePlot * getBodePlot();
    THDPlot * getTHDPlot();
    SpectrogramPlot * getSpectrogramPlot();
protected:
    BodePlot * bodePlot;
    THDPlot * thdPlot;
    SpectrogramPlot * spectrogramPlot;
};






