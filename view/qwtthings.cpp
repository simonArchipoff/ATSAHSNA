#include "qwtthings.h"

#include <QwtPlot>
#include <QwtPlotGrid>
#include <QwtLogScaleEngine>
#include <QwtPlotPanner>
#include <QwtPlotCanvas>

void qwtThingsSetFrequencyLogAxis(QwtPlot * plot,QwtAxisId axis){
  QwtPlotGrid* grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(false);
  grid->setMajorPen(Qt::black, 0, Qt::DotLine);
  grid->setMinorPen(Qt::gray,  0, Qt::DotLine);
  grid->attach(plot);
  auto log_scale = new QwtLogScaleEngine(10);
  QwtScaleDiv xDiv = log_scale->divideScale(10.0, 1000.0, 2, 10, 1.0);
  plot->setAxisScaleEngine(axis, log_scale);
  plot->setAxisMaxMajor(axis, 6);
  plot->setAxisMaxMinor(axis, 9);
}

FrequencyPlot::FrequencyPlot(QWidget * parent) : QwtPlot{parent}
{
  QwtPlotCanvas * canvas = new QwtPlotCanvas();
  setCanvas(canvas);

  // grid

  qwtThingsSetFrequencyLogAxis(this,QwtAxis::XBottom);


  // axes
  setAxisScale(QwtAxis::XBottom,20,20000);
  setAxisScale(QwtAxis::YLeft,-150,50);
  setAxisTitle(QwtAxis::XBottom, tr("fréquence"));

  (void) new QwtPlotPanner(canvas);
}
