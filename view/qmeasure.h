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

#include <signalResponse.h>
#include <signalHarmonics.h>
#include <signalSpectrogram.h>

#include "qcombobox.h"
#include "qspectrogram.h"
#include "qresponse.h"

#include "../constants.h"



class QParamDistortion : public QWidget{
  Q_OBJECT
public:
  QParamDistortion(QWidget * parent=nullptr);
  typedef struct ParamTHD Param;

  Param getParam();

signals:
  void start_measure_distortion(struct ParamTHD);

private:
  QDoubleSpinBox * frequency, *duration, *freq_min, *freq_max;
};

class StartMesure : public QWidget{
  Q_OBJECT
public:
  StartMesure(QWidget * parent=nullptr);
signals:
  void start_measure();
public:
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

    s->setOrientation(Qt::Vertical);
    qcontrol.reset(new QPARAM{s});
    qplot.reset(new QRESULT{s});
    temporalPlot.reset(new TemporalPlot());
    s->addWidget(qcontrol.data());
    s->addWidget(qplot.data());
    s->addWidget(temporalPlot.data());

    s->setSizes({1,3,1});
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  }

  typename QPARAM::Param getParam(){
    return qcontrol->getParam();
  }

  void setResult(const typename QRESULT::Result &r, QColor c){
    qplot->setResult(r,c);
    if constexpr (requires{r.raw_data;}){
      if(r.raw_data.inputs.size() > 0)
        temporalPlot->setInput(r.raw_data.inputs[0]);
      if(r.raw_data.outputs.size() > 0)
        temporalPlot->setOutput(r.raw_data.outputs[0]);
    }

  }

  QScopedPointer<TemporalPlot> temporalPlot;
  QScopedPointer<QPARAM> qcontrol;
  QScopedPointer<QRESULT> qplot;
};

typedef class QResult<QParamResponse,QDisplayResponse> QResponse;
typedef class QResult<QParamDistortion,QDisplayDistortion> QDistortion;
typedef class QResult<QParamSpectrogram,QDisplaySpectrogram> QSpectrogram;

