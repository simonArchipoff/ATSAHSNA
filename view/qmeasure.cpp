#include "qmeasure.h"

#include "../constants.h"
//#include "qcborvalue.h"
#include <QComboBox>
#include <QPushButton>
#include <QDebug>


StartMesure::StartMesure(QWidget * parent)
  :QWidget{parent}
  ,start_button{new QPushButton{tr("start"),this}}
{
  auto * l = new QHBoxLayout();
  l->addWidget(start_button);

  connect(start_button,&QPushButton::clicked, this,[this](){
      emit start_measure();
    });
  setLayout(l);
//  setMaximumSize(minimumSizeHint());
}



QParamDistortion::QParamDistortion(QWidget * parent):QWidget{parent}{
  auto b = new StartMesure{this};
  auto l = new QVBoxLayout();

  //setSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
  frequency = new QDoubleSpinBox();
  duration = new QDoubleSpinBox();
  freq_min = new QDoubleSpinBox();
  freq_max = new QDoubleSpinBox();

  for(auto * i : {frequency, freq_min, freq_max}){
      i->setRange(1,MAXFREQ);
    }
  duration->setRange(0.001,60);
  duration->setDecimals(6);
  freq_min->setValue(20);
  freq_max->setValue(20000);
  frequency->setValue(1000);
  duration->setValue(1);

  auto * form = new QFormLayout();

  form->addRow(tr("fundamental (Hz)"),frequency);
  form->addRow(tr("duration (s)"),duration);
  form->addRow(tr("minimal frequency (Hz)"),freq_min);
  form->addRow(tr("maximal frequency (Hz)"),freq_max);

  l->addWidget(b);
  l->addLayout(form);
  setLayout(l);
  connect(b,&StartMesure::start_measure, this,
          [this](){
      emit start_measure_distortion(this->getParam());
    });
//  setMaximumWidth(minimumSizeHint().width());
}

struct ParamHarmonics QParamDistortion::getParam(){
  ParamHarmonics f = {.frequency = frequency->value()
                     ,.duration = duration->value()
                     ,.freqMin  = freq_min->value()
                     ,.freqMax  = freq_max->value()};
  return f;
}


QParamResponse::QParamResponse(QWidget * parent)
    :QWidget{parent}
    ,fmin{new QSpinBox(this)}
    ,fmax{new QSpinBox(this)}
    ,duration{new QDoubleSpinBox(this)}
    ,typeMeasure{new QComboBox(this)}
{
  auto b = new StartMesure(this);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  auto l = new QVBoxLayout();

  setLayout(l);
  l->addWidget(typeMeasure.data());
  l->addWidget(b);



  QFormLayout * f = new QFormLayout;

  f->addRow(tr("frequency min"), fmin.data());
  f->addRow(tr("frequency max"), fmax.data());
  f->addRow(tr("duration"),duration.data());
  fmin->show();
  fmax->show();

  fmin->setMaximum(MAXFREQ);
  fmin->setMinimum(MINFREQ);
  fmin->setValue(20);
  fmax->setMaximum(MAXFREQ);
  fmax->setMinimum(MINFREQ);
  fmax->setValue(20000);

  duration->setMinimum(0.1);
  duration->setMaximum(20);
  duration->setValue(1);

  l->addLayout(f);

  //setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

  connect(b,&StartMesure::start_measure, this,[this](){
      emit start_measure_response(this->getParam());
  });
  //    setMaximumWidth(minimumSizeHint().width());
}


QParamResponse::Param QParamResponse::getParam(){
  auto fmin = this->fmin->value();
  auto fmax = this->fmax->value();
  auto duration = this->duration->value();
  return ParamResponse{fmin,
                       fmax,
                       duration};
}



/*
void QDisplayDistortion::setResult(const Result &r, QColor c){

}*/
