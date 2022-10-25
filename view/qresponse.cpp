#include "qresponse.h"

#include "qmeasure.h"


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

    typeMeasure->insertItem(signal_gen_type::CHIRP,tr("chirp"));
    typeMeasure->insertItem(signal_gen_type::IMPULSE,tr("impulse"));

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
          static_cast<signal_gen_type>(this->typeMeasure->currentIndex()),
          duration};
}
