#include "qspectrogram.h"


#include "qnamespace.h"
#include "qwt_axis.h"
#include "qwtthings.h"
#include "qwt_matrix_raster_data.h"

#include <QwtMatrixRasterData>
#include <QwtPlotSpectrogram>
#include <QVector>
#include <cmath>
#include <QPen>
#include <QwtColorMap>
#include <QwtInterval>
#include <QwtLogScaleEngine>
#include <QDebug>
qSpectrogram::qSpectrogram(QWidget * parent)
  :QwtPlot{parent}
{    qwtThingsSetFrequencyLogAxis(this,QwtAxis::YLeft);

}

class LinearColorMap : public QwtLinearColorMap
{
  public:
    LinearColorMap()
        : QwtLinearColorMap( Qt::darkBlue, Qt::darkRed )
    {
        setFormat( ( QwtColorMap::Format ) QwtColorMap::RGB);
        addColorStop(0.15,Qt::blue);
        addColorStop(0.33,Qt::green);
        addColorStop( 0.80, Qt::red );
        addColorStop(0.95,Qt::yellow);
        setMode(ScaledColors);
    }
};

#if 1
class RasterSpectro : public QwtRasterData {
public:
  RasterSpectro():QwtRasterData{},matrix(){

  }

  void setSpectrogramData(struct SpectrogramData &s){
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

  double value(double t, double f) const  QWT_OVERRIDE{
    if(f < freq.front() || f > freq.back())
      return qQNaN();
    if(t < 0 || t > duration)
      return qQNaN();

    double ti = (t/duration) * max_duration;
    for(uint i = 1; i < freq.size(); i++){
        if(f < freq[i]){
            auto foo = matrix.value(ti,i-1);
            return foo;
          }
      }
    return matrix.value(ti,freq.back());
  }

  QwtInterval interval (Qt::Axis axis) const QWT_OVERRIDE{

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

public:
  std::vector<double> freq;
  double duration;
  int max_duration;
  QwtMatrixRasterData matrix;
};

#else
class RasterSpectro : public QwtMatrixRasterData {
public:
  RasterSpectro():QwtMatrixRasterData(){
  }


  void setSpectrogramData(struct SpectrogramData &s){
    double high = s.frequencies.back();
    double low = s.frequencies.front();
    setValueMatrix(QVector<double>(s.data.begin(),s.data.end()),s.max_idx_time_rank);

    setInterval(Qt::XAxis,QwtInterval(0,s.duration));
    setInterval(Qt::YAxis,QwtInterval(low,high));

    double minValue = *std::min_element( std::begin(s.data), std::end(s.data) );
    double maxValue = *std::max_element( std::begin(s.data), std::end(s.data) );

    setInterval(Qt::ZAxis, QwtInterval(minValue, maxValue));
    }
  double value(double x, double y) const override{
    return QwtMatrixRasterData::value(x,y);
  }
};
#endif
void qSpectrogram::setSpectrogram(struct SpectrogramData &s){

  auto m = new  RasterSpectro(); //RasterSpectro(s);


  for(auto & i : s.data){
      i = 20*log10(i);
    }

  m->setSpectrogramData(s);


  auto spect = new QwtPlotSpectrogram;

  setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));

//m.setValueMatrix(QVector<double>::fromStdVector(s),log_n);

  spect->setRenderThreadCount( 0 );
  spect->setData(m);
  //spect->setDisplayMode( QwtPlotSpectrogram::ImageMode, true);
  //spect->setDefaultContourPen( QPen( Qt::black, 0 ));
  auto * colorMap = new LinearColorMap;
  //spect->setAlpha();
  QRectF r = spect->boundingRect();
//  qDebug() << r;

  //setAxisScale( QwtPlot::xBottom , r.left(), r.right(), 0.1);
  spect->attach(this);
  spect->setColorMap(colorMap);
  replot();
}

