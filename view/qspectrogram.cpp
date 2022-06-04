#include "qspectrogram.h"
#include "qwt_matrix_raster_data.h"

#include <QwtMatrixRasterData>
#include <QwtPlotSpectrogram>
#include <QVector>
#include <cmath>
#include <QPen>
#include <QwtColorMap>
#include <QwtInterval>
#include <QDebug>
qSpectrogram::qSpectrogram(QWidget * parent)
  :QwtPlot{parent}
{

}


void qSpectrogram::setSpectrogram(std::vector<double> &s){
  int n = s.size();
  int log_n = 0;
  {
    int tmp = n;
    while (tmp >>= 1) ++log_n;
  }
  auto m = new QwtMatrixRasterData;
  for(auto & i : s){
      i = 20*log10(i);
}
  m->setValueMatrix(QVector<double>(s.begin(),s.end()),log_n);
  m->setInterval( Qt::XAxis, QwtInterval(0, log_n) );
  m->setInterval( Qt::YAxis, QwtInterval(0, n/log_n) );

  double minValue = *std::min_element( std::begin(s), std::end(s) );
  double maxValue = *std::max_element( std::begin(s), std::end(s) );
  m->setInterval( Qt::ZAxis, QwtInterval(0, maxValue) );

  auto spect = new QwtPlotSpectrogram("foo");



//m.setValueMatrix(QVector<double>::fromStdVector(s),log_n);

  spect->setRenderThreadCount( 0 );
  spect->setData(m);
  spect->setDisplayMode( QwtPlotSpectrogram::ImageMode, true);
  spect->setDefaultContourPen( QPen( Qt::black, 0 ));
  QwtLinearColorMap * colorMap = new QwtLinearColorMap(Qt::white, Qt::black);
  spect->setColorMap(colorMap);
  spect->setAlpha(100);
  QRectF r = spect->boundingRect();
  qDebug() << r;

    setAxisScale( QwtPlot::xBottom , r.left(), r.right(), 0.1);
  spect->attach(this);
  //setColorMap( Plot::RGBMap );
  replot();
}

