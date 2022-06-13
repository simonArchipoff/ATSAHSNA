#pragma once
#include <QwtPlot>
#include <QwtAxis>

void qwtThingsSetFrequencyLogAxis(QwtPlot * plot,QwtAxisId axis);


class FrequencyPlot : public QwtPlot
{
  Q_OBJECT
public:
  FrequencyPlot(QWidget * parent);
};

