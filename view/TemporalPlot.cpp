#include "TemporalPlot.h"
#include <QVBoxLayout>
#include <QFormLayout>

TemporalPlot::TemporalPlot(QWidget * parent)
  : QWidget{parent}
  , start{new QSlider{this}}
  , len{new QSlider{this}}
  , input_data{}
  , output_data{}
  , plot{new QwtPlot{this}}
  , input_curve{new QwtPlotCurve{}}
  , output_curve{new QwtPlotCurve{}}
{
  input_curve->attach(plot.data());
  output_curve->attach(plot.data());

  QFormLayout * form = new QFormLayout();
  setLayout(form);
  form->addWidget(plot.data());
  form->addRow(tr("start"),start.data());
  form->addRow(tr("size"),len.data());
}

