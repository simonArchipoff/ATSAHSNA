#include "qspectrogram.h"


#include "qnamespace.h"

#include "qwtthings.h"

#include "qmeasure.h"

#include <QwtMatrixRasterData>
#include <QwtPlotSpectrogram>
#include <QwtAxis>
#include <QVector>
#include <QFormLayout>
#include <QScopedPointer>

#include <cmath>
#include <QPen>
#include <QwtColorMap>
#include <QwtInterval>
#include <QwtLogScaleEngine>
#include <QDebug>



QParamSpectrogram::QParamSpectrogram(QWidget * parent)
  :QWidget{parent}
  ,nb_octaves{new QSpinBox}
  ,resolution{new QSpinBox}
  ,paramResponse{new QParamResponse{this}}
{
  resolution->setRange(1,256);
  resolution->setValue(16);
  nb_octaves->setRange(1,15);
  nb_octaves->setValue(10);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  auto l = new QVBoxLayout();

  setLayout(l);
  l->addWidget(paramResponse.data());
  //l->addWidget(b);

  QFormLayout * f = new QFormLayout;
  f->addRow(tr("nombre d'octaves"), nb_octaves.data());
  f->addRow(tr("résolution"),resolution.data());

  l->addLayout(f);

  connect(paramResponse.data(),&QParamResponse::start_measure_response
          , this
          , [this](auto p,auto b){
      auto ps = this->getParam();
      ps.freqMax = p.freqMax;
      ps.freqMin = p.freqMin;
      ps.mode = p.mode;
      emit start_measure_spectrogram(ps,b);
    });
}



ParamSpectrogram QParamSpectrogram::getParam(){
  auto r = ParamSpectrogram{
      .nb_octave = this->nb_octaves->value()
      ,.resolution = this->resolution->value()
    };
  return r;
}


RasterSpectro::RasterSpectro():QwtRasterData{},matrix(){
}

void RasterSpectro::setSpectrogramData(const struct ResultSpectrogram &s){
  freq = s.frequencies;
  duration = s.duration;
  max_duration = s.max_idx_time_rank;
  //double high = s.frequencies.back();
  //double low = s.frequencies.front();
  matrix.setValueMatrix(QVector<double>(s.data.begin(),s.data.end()),s.max_idx_time_rank);

  matrix.setInterval(Qt::XAxis,QwtInterval(0,s.max_idx_time_rank));
  matrix.setInterval(Qt::YAxis,QwtInterval(0,s.max_freq_rank));

  double minValue = *std::min_element( std::begin(s.data), std::end(s.data) );
  double maxValue = *std::max_element( std::begin(s.data), std::end(s.data) );

  matrix.setInterval(Qt::ZAxis, QwtInterval(minValue, maxValue));
}

double RasterSpectro::value(double t, double f) const {
  if(f < freq.front() || f > freq.back())
    return qQNaN();
  if(t < 0 || t > duration)
    return qQNaN();

  double ti = (t/duration) * max_duration;
  for(uint i = 0; i < freq.size(); i++){
      if(f < freq[i]){
          auto foo = matrix.value(ti,i);
          return foo;
        }
    }
  return matrix.value(ti,freq.back());
}

QwtInterval RasterSpectro::interval (Qt::Axis axis) const {

  switch(axis){
    case(Qt::XAxis):
      return QwtInterval(0,duration);
    case(Qt::YAxis):
      return QwtInterval(freq.front(),freq.back());
    default:
      auto i = matrix.interval(axis);
      i.setMinValue(i.maxValue() - 100);
      return i;
    }
}



QDisplaySpectrogram::QDisplaySpectrogram(QWidget * parent)
  :QwtPlot{parent}
  , rasterspectro{new RasterSpectro}//RasterSpectro(s);
  , qwtplotspectrogram{new QwtPlotSpectrogram}
{
  qwtThingsSetFrequencyLogAxis(this,QwtAxis::YLeft);

}




void QDisplaySpectrogram::setResult(const QDisplaySpectrogram::Result &s,QColor &c) {
  Result copy_s{s};
  for(auto & i : copy_s.data){
      i = 20*log10(i);
    }
  rasterspectro->setSpectrogramData(copy_s);
  setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
//m.setValueMatrix(QVector<double>::fromStdVector(s),log_n);
  qwtplotspectrogram->setRenderThreadCount(4);
  qwtplotspectrogram->setData(rasterspectro);
  //spect->setDisplayMode( QwtPlotSpectrogram::ImageMode, true);
  //spect->setDefaultContourPen( QPen( Qt::black, 0 ));
  auto * colorMap = new ColorMapViridis;
  //spect->setAlpha();
  //QRectF r = qwtplotspectrogram->boundingRect();
//  qDebug() << r;

  //setAxisScale( QwtPlot::xBottom , r.left(), r.right(), 0.1);
  qwtplotspectrogram->attach(this);
  qwtplotspectrogram->setColorMap(colorMap);

  replot();
}

