#pragma once

#include <QWidget>
#include <QwtPlotSpectrogram>
#include <QwtPlot>

#include <QSpinBox>

#include "backend/spectrogram.h"
#include <constants.h>


class QParamSpectrogram : public QWidget {
  Q_OBJECT
public:
  QParamSpectrogram(QWidget * parent);
  typedef struct ParamSpectrogram Param;
  Param getParam();
signals:
  void start_measure_spectrogram(struct ParamSpectrogram, backend_type);
private:
  QScopedPointer<QSpinBox> nb_octaves,resolution;
};




class QDisplaySpectrogram : public QwtPlot
{
  Q_OBJECT
public:
  QDisplaySpectrogram(QWidget * parent=nullptr);
  typedef ResultSpectrogram Result;
  void setResult(const Result &s,QColor &c);
};
