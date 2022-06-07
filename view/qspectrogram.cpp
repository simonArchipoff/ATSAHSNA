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

class LinearColorMap : public QwtLinearColorMap
{
  public:
    LinearColorMap()
        : QwtLinearColorMap( Qt::darkCyan, Qt::red )
    {
        setFormat( ( QwtColorMap::Format ) QwtColorMap::RGB);

        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );
    }
};

void qSpectrogram::setSpectrogram(struct SpectrogramData &spectd){
  auto s = spectd.data;
  int n = s.size();
  int log_n = 0;
  {
    int tmp = n;
    while (tmp >>= 1) ++log_n;
  }
  auto m = new QwtMatrixRasterData;

  double minValue = *std::min_element( std::begin(s), std::end(s) );
  double maxValue = *std::max_element( std::begin(s), std::end(s) );
  for(auto & i : s){
    //  i = 20*log10(i);
    }

  m->setValueMatrix(QVector<double>(s.begin(),s.end()),44100);
  m->setInterval( Qt::XAxis, QwtInterval(0, 44100) );
  m->setInterval( Qt::YAxis, QwtInterval(0, s.size() / 44100) );


  m->setInterval( Qt::ZAxis, QwtInterval(minValue, maxValue) );

  auto spect = new QwtPlotSpectrogram("foo");



//m.setValueMatrix(QVector<double>::fromStdVector(s),log_n);

  spect->setRenderThreadCount( 0 );
  spect->setData(m);
  //spect->setDisplayMode( QwtPlotSpectrogram::ImageMode, true);
  //spect->setDefaultContourPen( QPen( Qt::black, 0 ));
  auto * colorMap = new LinearColorMap;
  spect->setAlpha(100);
  QRectF r = spect->boundingRect();
  qDebug() << r;

  //setAxisScale( QwtPlot::xBottom , r.left(), r.right(), 0.1);
  spect->attach(this);
  //setColorMap( Plot::RGBMap );
  replot();
}

