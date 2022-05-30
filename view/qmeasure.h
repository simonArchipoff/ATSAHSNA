#pragma once


#include "BodePlot.h"
#include "TemporalPlot.h"
#include <QDockWidget>
#include <QSplitter>
#include <QScopedPointer>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QBoxLayout>

#include "backend/measure.h"
#include "constants.h"



class QParamResponse : public QWidget {
  Q_OBJECT
public:
  QParamResponse(QWidget * parent);
  typedef struct ParamResponse Param;
  Param getParam();
signals:
  void start_measure_response(struct ParamResponse, backend_type);
private:
  QScopedPointer<QSpinBox> fmin,fmax;
};

class QDisplayResponse : public BodePlot {
  Q_OBJECT
public:
  typedef ResultResponse Result;
  QDisplayResponse(QWidget * parent):BodePlot{parent}{
  };

  void setResult(const Result &r, QColor c);
};


class QParamDistortion : public QWidget{
  Q_OBJECT
public:
  QParamDistortion(QWidget * parent=nullptr);
  typedef struct ParamTHD Param;

  Param getParam();

signals:
  void start_measure_distortion(struct ParamTHD, backend_type);

private:
  QDoubleSpinBox * frequency, *duration, *freq_min, *freq_max;
};

class StartMesure : public QWidget{
  Q_OBJECT
public:
  StartMesure(QWidget * parent=nullptr);
signals:
  void start_measure(backend_type p);
public:
  QComboBox * backends;
  QPushButton * start_button;
};

class QDisplayDistortion : public THDPlot{
  Q_OBJECT
public:
  typedef ResultTHD Result;
  QDisplayDistortion(QWidget * parent=nullptr);
};


template<class QPARAM,class QRESULT>
class QResult : public QWidget {
public:
  QResult():QWidget(){
    auto * l = new QVBoxLayout;
    auto s = new QSplitter(this);
    l->addWidget(s);
    setLayout(l);
    //s->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    s->setOrientation(Qt::Horizontal);
    qcontrol.reset(new QPARAM{s});
    qplot.reset(new QRESULT{s});
    temporalPlot.reset(new TemporalPlot());
    s->addWidget(qcontrol.data());
    s->addWidget(qplot.data());
    l->addWidget(temporalPlot.data());

  }

  typename QPARAM::Param getParam(){
    return qcontrol->getParam();
  }

  void setResult(const typename QRESULT::Result &r, QColor c){
    qplot->setResult(r,c);
    temporalPlot->setInput(r.raw_data.inputs[0]);
    temporalPlot->setOutput(r.raw_data.outputs[0]);
  }

  QScopedPointer<TemporalPlot> temporalPlot;
  QScopedPointer<QPARAM> qcontrol;
  QScopedPointer<QRESULT> qplot;
};

typedef class QResult<QParamResponse,QDisplayResponse> QResponse;
typedef class QResult<QParamDistortion,QDisplayDistortion> QDistortion;


