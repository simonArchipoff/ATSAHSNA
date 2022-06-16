#include "qresults.h"
#include <QSplitter>
#include <QTabWidget>



QResults::QResults(QWidget * parent)
  :
  QTabWidget{parent}
  ,qresponse{new QResponse}
  ,qdistortion{new QDistortion}
  ,qspectrogram{new QSpectrogram}
{
  this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  qresponse->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  addTab(qresponse.data(),tr("response"));
  addTab(qdistortion.data(),tr("distortion"));  
  addTab(qspectrogram.data(),tr("spectrogram"));


  connect(qresponse->qcontrol.data(),&QParamResponse::start_measure_response,this,
          [this](struct ParamResponse p, auto b){ emit request_response(p,b);});
  connect(qdistortion->qcontrol.data(),&QParamDistortion::start_measure_distortion,this,
          [this](auto p, auto b){emit request_distortion(p,b);});
  connect(qspectrogram->qcontrol.data(), &QParamSpectrogram::start_measure_spectrogram,this,
          [this](struct ParamSpectrogram p, auto b){emit request_spectrogram(p,b);});
}


void QResults::setResult(const struct ResultTHD &r, QColor c){
  qdistortion.data()->setResult(r,c);

}
void QResults::setResult(const struct ResultResponse & r, QColor c){
  qresponse.data()->setResult(r,c);
}

void QResults::setResult(const struct ResultSpectrogram & r, QColor c){
  qspectrogram.data()->setResult(r,c);
}
