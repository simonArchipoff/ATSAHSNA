#include "qspectrogram.h"
#include "qnamespace.h"
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
{

}

class LinearColorMap : public QwtLinearColorMap
{
  public:
    LinearColorMap()
        : QwtLinearColorMap( Qt::black, Qt::white )
    {
        setFormat( ( QwtColorMap::Format ) QwtColorMap::RGB);
        addColorStop(0.15,Qt::blue);
        addColorStop(0.33,Qt::green);
        addColorStop( 0.80, Qt::red );
        addColorStop(0.95,Qt::yellow);
        setMode(ScaledColors);
    }
};



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
};

void qSpectrogram::setSpectrogram(struct SpectrogramData &s){


  int log_n = 0;
  {
    int tmp = s.max_idx_time_rank;
    while (tmp >>= 1) ++log_n;
  }
  auto m = new  RasterSpectro(); //RasterSpectro(s);


  for(auto & i : s.data){
    //  i = 20*log10(i);
    }

  m->setSpectrogramData(s);


  auto spect = new QwtPlotSpectrogram("foo");

  //setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(2);

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

