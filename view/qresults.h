#pragma once


#include "qmeasure.h"
#include "qspectrogram.h"
#include <measure.h>

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
signals:
  void request_response   (ParamResponse p,     backend_type);
  void request_distortion (ParamTHD p,          backend_type);
  void request_spectrogram(ParamSpectrogram p, backend_type);
protected:
  QPointer<QResponse> qresponse;
  QPointer<QDistortion> qdistortion;
  QPointer<QSpectrogram> qspectrogram;
};


