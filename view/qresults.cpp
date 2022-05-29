#include "qresults.h"
#include <QSplitter>
#include <QTabWidget>



QResults::QResults(QWidget * parent) :
  QTabWidget{parent}
  ,qdistortion{new QDistortion}
  ,qresponse{new QResponse}
{
  this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  qresponse->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  addTab(qresponse.data(),tr("response"));
  addTab(qdistortion.data(),tr("distortion"));  



  connect(qresponse->qcontrol.data(),&QParamResponse::start_measure_response,this,
          [this](struct ParamResponse p, auto b){ emit request_response(p,b);});
  connect(qdistortion->qcontrol.data(),&QParamDistortion::start_measure_distortion,this,
          [this](auto p, auto b){emit request_distortion(p,b);});
}


void QResults::setResult(const struct ResultTHD &r, QColor c){
  qdistortion.data()->qplot->setResult(r,c);

}
void QResults::setResult(const struct ResultResponse & r, QColor c){
  qresponse.data()->qplot->setResult(r,c);

}
