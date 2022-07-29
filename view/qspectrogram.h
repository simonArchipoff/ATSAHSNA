#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QComboBox>

#include <qwt_plot_spectrogram.h>
#include <qwt_plot.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_interval.h>
#include <qwt_plot_zoomer.h>

#include "qresponse.h"

#include "../constants.h"


class QParamSpectrogram : public QWidget {
  Q_OBJECT
public:
  QParamSpectrogram(QWidget * parent);
  typedef struct ParamSpectrogram Param;
  Param getParam();
signals:
  void start_measure_spectrogram(struct ParamSpectrogram);
private:
  QScopedPointer<QSpinBox> nb_octaves,resolution;
  QScopedPointer<QParamResponse> paramResponse;
};


class RasterSpectro : public QwtRasterData {
public:
  RasterSpectro();

  void setSpectrogramData(const struct ResultSpectrogram &s);
  double value(double t, double f) const  QWT_OVERRIDE;
  QwtInterval interval (Qt::Axis axis) const QWT_OVERRIDE;

protected:
  std::vector<double> freq;
  double duration;
  int max_duration;
  QwtMatrixRasterData matrix;

};

class QDisplaySpectrogram : public QwtPlot
{
  Q_OBJECT
public:
  QDisplaySpectrogram(QWidget * parent=nullptr);
  typedef ResultSpectrogram Result;
  void setResult(const Result &s,QColor &c);
protected:
  RasterSpectro * rasterspectro;
  QwtPlotSpectrogram * qwtplotspectrogram;
  QwtPlotZoomer * zoomer;
};
