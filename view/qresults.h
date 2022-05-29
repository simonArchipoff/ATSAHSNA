#pragma once

#include "qscopedpointer.h"
#include "qmeasure.h"
#include "backend/measure.h"

#include <QObject>
#include <QTabWidget>
#include <QScopedPointer>
#include <QSplitter>


/*! This class display the results of the computations
 */
class QResults : public QTabWidget {
  Q_OBJECT
public:
  QResults(QWidget * parent);

  void setResult(const struct ResultTHD&, QColor c);
  void setResult(const struct ResultResponse&, QColor c);

signals:
    void request_response(ParamResponse p, backend_type);
    void request_distortion(ParamTHD p,    backend_type);

protected:
  QScopedPointer<QDistortion> qdistortion;
  QScopedPointer<QResponse> qresponse;
};

