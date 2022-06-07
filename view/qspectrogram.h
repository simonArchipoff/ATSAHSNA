#pragma once

#include <QWidget>
#include <QwtPlotSpectrogram>
#include <QwtPlot>
#include "backend/spectrogram.h"


class qSpectrogram : public QwtPlot
{
  Q_OBJECT
public:
  qSpectrogram(QWidget * parent=nullptr);

  void setSpectrogram(struct SpectrogramData &s);

};
