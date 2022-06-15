#pragma once
#include <QwtPlot>
#include <QwtAxis>
#include <QwtLinearColorMap>
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
