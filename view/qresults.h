#pragma once

#include "qmeasure.h"
#include "qspectrogram.h"

#include <signalResponse.h>
#include <signalHarmonics.h>
#include <signalSpectrogram.h>

#include <QObject>
#include <QTabWidget>
#include <QPointer>
#include <QSplitter>


/*! This class display the results of the computations
 */
class QResults : public QTabWidget {
  Q_OBJECT
public:
  QResults(QWidget * parent);

  void setResult(const struct ResultTHD&, QColor c);
  void setResult(const struct ResultResponse&, QColor c);
  void setResult(const struct ResultSpectrogram&, QColor c);

  void click_start_on_current_tab();
signals:
  void request_response   (ParamResponse p);
  void request_distortion (ParamTHD p);
  void request_spectrogram(ParamSpectrogram p);
protected:
  QPointer<QResponse> qresponse;
  QPointer<QDistortion> qdistortion;
  QPointer<QSpectrogram> qspectrogram;
};


