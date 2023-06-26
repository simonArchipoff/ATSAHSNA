#pragma once

#include <QWidget>

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
};


