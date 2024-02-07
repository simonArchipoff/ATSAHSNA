#pragma once


#include <QDockWidget>
#include <QSplitter>
#include <QScopedPointer>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QBoxLayout>

#include <Response.h>
#include <Harmonics.h>
#include <Spectrogram.h>



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

class QParamDistortion : public QWidget{
  Q_OBJECT
public:
  QParamDistortion(QWidget * parent=nullptr);
  typedef struct ParamHarmonics Param;

  Param getParam();

signals:
  void start_measure_distortion(struct ParamHarmonics);

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
/*
template<class QPARAM>
class QResult : public QWidget {
public:
  QResult():QWidget(){
    auto * l = new QVBoxLayout;
    auto s = new QSplitter(this);

    l->addWidget(s);
    setLayout(l);

    s->setOrientation(Qt::Vertical);
    qcontrol.reset(new QPARAM{s});
    s->addWidget(qcontrol.data());
    s->setSizes({1,3,1});
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  }

  typename QPARAM::Param getParam(){
    return qcontrol->getParam();
  }

  QScopedPointer<QPARAM> qcontrol;
};


typedef class QResult<QParamResponse>   QResponse;
typedef class QResult<QParamDistortion> QDistortion;
//typedef class QResult<QParamSpectrogram,QDisplaySpectrogram> QSpectrogram;
*/
