#include "qmeasure.h"
#include "backend/measure.h"
#include "constants.h"
#include "qcborvalue.h"
#include "qpushbutton.h"
#include <QDebug>

QParamResponse::QParamResponse(QWidget * parent)
  :QWidget{parent}
  ,fmin{new QSpinBox(this)}
  ,fmax{new QSpinBox(this)}
{
    auto b = new StartMesure(this);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    auto l = new QVBoxLayout();

    setLayout(l);
    l->addWidget(b);

    QFormLayout * f = new QFormLayout;

    f->addRow("fréquence min", fmin.data());
    f->addRow("fréquence max", fmax.data());
    fmin->show();
    fmax->show();

    fmin->setMaximum(MAXFREQ);
    fmin->setMinimum(MINFREQ);
    fmin->setValue(10);
    fmax->setMaximum(MAXFREQ);
    fmax->setMinimum(MINFREQ);
    fmax->setValue(20000);

    l->addLayout(f);

    //setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    connect(b,&StartMesure::start_measure, this,[this](auto b){
        emit start_measure_response(this->getParam(),b);
    });
    setMaximumWidth(minimumSizeHint().width());
}


StartMesure::StartMesure(QWidget * parent)
  :QWidget{parent}
  ,backends{new QComboBox{this}}
  ,start_button{new QPushButton{"start",this}}
{
  auto * l = new QHBoxLayout();
  l->addWidget(start_button);
  l->addWidget(backends);


  backends->insertItem(backend_type::FAUST,"faust");
  backends->insertItem(backend_type::JACK,"jack");

  connect(start_button,&QPushButton::clicked, this,[this](){
      emit start_measure((backend_type) backends->currentIndex());
    });
  setLayout(l);
  setMaximumSize(minimumSizeHint());
}


QParamResponse::Param QParamResponse::getParam(){
  return ParamResponse{ this->fmin->value(),
          this->fmax->value(),
          ParamResponse::IMPULSE};
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
      i->setRange(1,20000);
    }
  duration->setRange(0.5,10);
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
  connect(b,&StartMesure::start_measure, this,[this](auto b){emit start_measure_distortion(this->getParam(),b);});
  setMaximumWidth(minimumSizeHint().width());
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
  BodePlot::setResult(r.response, c);
}
