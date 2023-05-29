#pragma once
#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include "BodePlot.h"
#include <signalResponse.h>
class QParamResponse : public QWidget {
  Q_OBJECT
public:
  QParamResponse(QWidget * parent);
  typedef struct ParamResponse Param;
  Param getParam();
signals:
  void start_measure_response(struct ParamResponse);
private:
  QScopedPointer<QSpinBox> fmin,fmax;
  QScopedPointer<QDoubleSpinBox> duration;
  QScopedPointer<QComboBox> typeMeasure;
};

class QDisplayResponse : public BodePlot {
  Q_OBJECT
public:
  typedef ResultResponse Result;
  QDisplayResponse(QWidget * parent=nullptr):BodePlot{parent}{
  };

  void setResult(const Result &r, QColor c);
};
