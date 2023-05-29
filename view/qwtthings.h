#pragma once
#include <qwt_plot.h>
#include <qwt_axis.h>
#include <qwt_color_map.h>
void qwtThingsSetFrequencyLogAxis(QwtPlot * plot,QwtAxisId axis);


class FrequencyPlot : public QwtPlot
{
  Q_OBJECT
public:
  FrequencyPlot(QWidget * parent);
};


class ColorMapViridis : public QwtLinearColorMap{
public:
  ColorMapViridis();
};

class ColorMapBlackBody : public QwtLinearColorMap{
public:
  ColorMapBlackBody();
};
