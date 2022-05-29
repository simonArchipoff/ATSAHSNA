#include "BodePlot.h"

#include <cmath>

#include <QwtMath>
#include <QwtScaleEngine>
#include <QwtSymbol>
#include <QwtPlotGrid>
#include <QwtPlotMarker>
#include <QwtPlotCurve>
#include <QwtLegend>
#include <QwtText>
#include <QwtPlotCanvas>
#include <QwtAxisId>
#include <QObject>
#include <QEvent>
#include <QWheelEvent>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>
#include <QwtScaleMap>
#include <algorithm>
#include <QDebug>


BodeCurve::BodeCurve(const QColor c) : color(c) {
    c_phase.reset(new QwtPlotCurve());
    c_phase->setRenderHint(QwtPlotItem::RenderAntialiased);
    c_phase->setPen(c,1,Qt::DotLine);
    c_phase->setYAxis(QwtAxis::YRight);

    c_amplitude.reset(new QwtPlotCurve());
    c_amplitude->setRenderHint(QwtPlotItem::RenderAntialiased);
    c_amplitude->setPen(c,1,Qt::SolidLine);
    c_amplitude->setStyle(QwtPlotCurve::Lines);
    c_amplitude->setYAxis(QwtAxis::YLeft);

    displayed=true;
};

void BodeCurve::attach(QwtPlot * p){
    c_phase->attach(p);
    c_amplitude->attach(p);
}

double BodeCurve::maxFrequency(){
    return c_phase->maxXValue();
}

double BodeCurve::minFrequency(){
    return c_phase->minXValue();
}

void BodePlot::setResult(const FDF &c, const QColor color){
    setAutoReplot(false);
    QString colname = color.name();
    if(!curves.contains(colname))
      curves.insert(colname, new BodeCurve(color));
    BodeCurve * bc = curves[colname];
    bc->attach(this);
    bc->setCurve(c);

    QwtPlot::setAxisScale(QwtAxis::XBottom,std::max<double>(10,bc->minFrequency()),bc->maxFrequency());
    setAxisScale(QwtAxis::YLeft,std::max(-100.,std::min(bc->minAmplitude,minAmplitude)),std::max(bc->maxAmplitude,maxAmplitude));
    setAutoReplot(true);
    replot();
}



void BodeCurve::setCurve(const FDF &c){
    const int s = c.getResponse().size();
    const int s2 = s/2;

    vector<double> a = c.getAmplitude20log10();
    vector<double> p = c.getPhase(false);
    vector<double> f = c.getFrequency();
#if 0
    for(uint  i = 0; i < s2 ; i++){
        qDebug() <<  a[i] << " " << f[i];
    }
#endif
  c_amplitude->setSamples(f.data(), a.data(),s2);
  c_phase->setSamples(f.data(),p.data(),s2);

  maxAmplitude = *std::max_element(a.begin(),a.end());
  minAmplitude = *std::min_element(a.begin(),a.end());
}


FrequencyPlot::FrequencyPlot(QWidget * parent) : QwtPlot{parent}
{
  QwtPlotCanvas * canvas = new QwtPlotCanvas();
  setCanvas(canvas);

  // grid
  QwtPlotGrid* grid = new QwtPlotGrid;
  grid->enableXMin( true );
  grid->enableYMin(false);
  grid->setMajorPen(Qt::black, 0, Qt::DotLine);
  grid->setMinorPen(Qt::gray,  0, Qt::DotLine);
  grid->attach(this);
  auto log_scale = new QwtLogScaleEngine(10);
  QwtScaleDiv xDiv = log_scale->divideScale(10.0, 1000.0, 2, 10, 1.0);
  setAxisScaleEngine(QwtPlot::xBottom, log_scale);

  setAxisMaxMajor(QwtAxis::XBottom, 6);
  setAxisMaxMinor(QwtAxis::XBottom, 9);


  // axes
  setAxisScale(QwtAxis::XBottom,20,20000);
  setAxisScale(QwtAxis::YLeft,-150,50);
  setAxisTitle(QwtAxis::XBottom, tr("fréquence"));

  (void) new QwtPlotPanner(canvas);
}


BodePlot::BodePlot(QWidget* parent) : FrequencyPlot{parent}
{
    setAutoReplot(false);

    setAxisVisible(QwtAxis::YRight);
    setAxisTitle(QwtAxis::YLeft, tr("amplitude"));
    setAxisTitle(QwtAxis::YRight, tr("phase"));
    setAxisScale(QwtAxis::YRight, -180, 180);
    QwtPlot::setAxisScale(QwtAxis::YLeft,-100,20);
    setAxisAutoScale(QwtAxis::YLeft);

    setAutoReplot(true);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}

THDPlot::THDPlot(QWidget * parent):FrequencyPlot(parent){
  textResult = new QwtPlotTextLabel();
  textResult->attach(this);
}


static QString thdResultString(const ResultTHD & r){
  const ParamTHD & p = r.params;
  auto s = QString("THD+N@%1Hz = %2%\n(%3Hz ~ %4Hz)")
      .arg(p.frequency).arg(QString::number(r.thdRate*100, 'f', 6) ).arg(p.freqMin).arg(p.freqMax);
  return s;
}


void THDPlot::setResult(const ResultTHD &r, QColor color){
  QString colname = color.name();
  if(!curves.contains(colname))
    curves.insert(colname, new QwtPlotCurve());
  QwtPlotCurve * pc = curves[colname];

  pc->setRenderHint(QwtPlotItem::RenderAntialiased);
  pc->setPen(color,1,Qt::SolidLine);
  pc->setStyle(QwtPlotCurve::Lines);
  pc->setYAxis(QwtAxis::YLeft);

  pc->attach(this);

  auto a = r.harmonicSpectrum.getAmplitude20log10();
  auto f = r.harmonicSpectrum.getFrequency();

  pc->setSamples(f.data(), a.data(), f.size()/2);
  QwtText t(thdResultString(r));
  t.setRenderFlags(Qt::AlignRight | Qt::AlignTop);
  QFont font;
  font.setBold( false );
  t.setFont(font);
  t.setColor(color);
  textResult->setText(t);

  setAxisScale(QwtAxis::YLeft,r.harmonicSpectrum.getMinAmplitude20log10(), r.harmonicSpectrum.getMaxAmplitude20log10());

  setAutoReplot(true);
  replot();
}

