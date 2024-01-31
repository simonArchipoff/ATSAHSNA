#pragma once




#include <QTabWidget>
#include <frequencyplot.h>

#include <QColor>
#include <QVector>

#include <qcustomplot.h>

#include <Response.h>
#include <Harmonics.h>

#include <SpectrogramPlot.h>



class BodePlot : public FrequencyPlot
{
    Q_OBJECT
public:
    BodePlot(QWidget*parent);
    //void setResult(std::variant<const std::vector<ResultResponse>> & r);
    void setResult(const ResultResponse &r, QString name, QColor c);
};


