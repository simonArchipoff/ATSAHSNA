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
  duration->setRange(0.1,10);
  freq_min->setValue(10);
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

struct ParamTHD QParamDistortion::getParam(){
  ParamTHD f = {.frequency = frequency->value()
                ,.duration = duration->value()
                ,.freqMin  = freq_min->value()
                ,.freqMax  = freq_max->value()};
  return f;
}

QDisplayDistortion::QDisplayDistortion(QWidget * parent):THDPlot{parent}{
}



/*
void QDisplayDistortion::setResult(const Result &r, QColor c){

}*/

void QDisplayResponse::setResult(const Result&r, QColor c){
  BodePlot::setResult(r.response, c,r.params.freqMin,r.params.freqMax);
}
