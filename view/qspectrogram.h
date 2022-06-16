#pragma once

#include <QWidget>
#include <QwtPlotSpectrogram>
#include <QwtPlot>
#include <QwtMatrixRasterData>
#include <QwtInterval>
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
};
