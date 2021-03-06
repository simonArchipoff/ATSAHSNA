#pragma once

#include <QWidget>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <QSlider>
#include <signalAnalysis.h>


class TemporalPlot : public QWidget
{
  Q_OBJECT
public:
  TemporalPlot(QWidget * parent=nullptr);

  void setInput(const VD &in);
  void setOutput(const VD &out);

  void plot_data(uint start, uint len);

protected:

  QScopedPointer<QSlider> start, len;
  VD input_data;
  VD output_data;
  QScopedPointer<QwtPlot> plot;
  QScopedPointer<QwtPlotCurve> input_curve, output_curve;
};


