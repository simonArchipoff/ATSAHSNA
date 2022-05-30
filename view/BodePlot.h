#pragma once

#include <QMap>
#include <QWidget>
#include <QSlider>
#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtPlotMarker>
#include <vector>
#include <QwtPlotTextLabel>
#include "backend/DTF.h"
#include "backend/measure.h"
#include "qscopedpointer.h"
#include "qwt_plot_curve.h"

class BodeCurve{
public:
  //BodeCurve(const BodeCurve&) = default;
  BodeCurve(const QColor);
  void setCurve(const FDF&);
  void attach(QwtPlot *);
  double maxFrequency();
  double minFrequency();

  QSharedPointer<QwtPlotCurve> c_amplitude, c_phase;
  QColor color;
  double maxAmplitude,minAmplitude;
  bool displayed;
};


class FrequencyPlot : public QwtPlot
{
  Q_OBJECT
public:
  FrequencyPlot(QWidget * parent);
};

class BodePlot : public FrequencyPlot
{
  Q_OBJECT
public:
  BodePlot(QWidget * parent);
  void setResult(const FDF &, QColor c);
  void removeCurve(QColor c);
  //QString represent color, because they are orderer, qcolor are not, maybe I could just take a name…
  QMap<QString,BodeCurve *> curves;

  double maxAmplitude = 10, minAmplitude=-100;
};

class THDPlot : public FrequencyPlot
{
public:
  THDPlot(QWidget* parent);
  void setResult(const ResultTHD &, QColor c);
  //void setThdResult

  QMap<QString,QwtPlotCurve *> curves;
  QwtPlotTextLabel *textResult;
};
