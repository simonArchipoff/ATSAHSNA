#include "BodePlot.h"
#include "qwtthings.h"
#include "signalAnalysis.h"

#include <cmath>

#include <QObject>
#include <QEvent>
#include <QWheelEvent>

#include <qwt_math.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_canvas.h>
#include <qwt_axis_id.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_map.h>
#include <qwt_scale_engine.h>

#include <algorithm>
#include <QDebug>


void clean_spectrum(VD &s){
  for(uint i = 0; i < s.size(); i++){
      if(std::isnan(s[i]) || std::isinf(s[i]) || s[i] < -400){
          if(i == 0){
              s[i] = s[i+1];
            } else {
              s[i] = s[i-1];
            }
        }
    }
}


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



void BodePlot::setResult(const FDF &c, const QColor color,double freqMin, double freqMax){
    setAutoReplot(false);
    QString colname = color.name();
    if(!curves.contains(colname))
      curves.insert(colname, new BodeCurve(color));
    BodeCurve * bc = curves[colname];
    bc->attach(this);
    bc->setCurve(c,freqMin,freqMax);

    //QwtPlot::setAxisScale(QwtAxis::XBottom,std::max<double>(10,bc->minFrequency()),bc->maxFrequency());
    QwtPlot::setAxisScale(QwtAxis::XBottom,bc->minFrequency,bc->maxFrequency);
    QwtPlot::setAxisScale(QwtAxis::YLeft,bc->minAmplitude,bc->maxAmplitude);
    setAutoReplot(true);
    replot();
}


uint find_first_greater_than(vector<double> v, double e){
  for(uint i = 0; i < v.size(); i++){
      if(v[i] >= e)
        return i;
    }
  return 0;
}
uint find_last_smaller_than(vector<double> v, double e){
  for(uint i = 0 ; i < v.size(); i++){
      auto idx = v.size() - 1 - i;
      if(v[idx] > e)
        return idx;
    }
  return v.size() -1;
}

void BodeCurve::setCurve(const FDF &c, double minF, double maxF){
    //auto r = c.getDecimatedAmplitude20log10PhaseFrequency(c.getFrequency().size());
    FDFLOG l{c,10};
    l.trimHF(maxF);
    l.trimLF(minF);
    vector<double> a = l.getAmplitude20log10();
      //get<0>(r);
    vector<double> p = l.getPhase();
      //get<1>(r);
    vector<double> f = l.getFrequency();
      //  get<2>(r);
    //p = try_make_phase_continuous(p);
    //p = decimation_log(p,10000);
    //a = decimation_log(a,10000);
    //vector<double> f = c.getFrequency();

    const int s = f.size();
    //f = decimation_log(f,10000);

    for(uint i = 0; i < a.size(); i++){
        if(a[i] < -100)
          p[i]=NAN;
      }

  c_amplitude->setSamples(f.data(), a.data(),s);
  c_phase->setSamples(f.data(),p.data(),s);

  minFrequency = f[1];
  maxFrequency = f[f.size() - 1];
  int min = find_first_greater_than(f,minF);
  int max = find_last_smaller_than(f,maxF);
  assert(min < max);

  maxAmplitude = *std::max_element(a.begin() + min, a.begin() + max);
  minAmplitude = *std::min_element(a.begin() + min, a.begin() + max);
}



BodePlot::BodePlot(QWidget* parent) : FrequencyPlot{parent}
{
    setAutoReplot(false);

    setAxisVisible(QwtAxis::YRight);
    setAxisTitle(QwtAxis::YLeft, tr("amplitude"));
    setAxisTitle(QwtAxis::YRight, tr("phase"));

    QwtPlot::setAxisScale(QwtAxis::YLeft,-50,6);
    setAxisScale(QwtAxis::YRight, -185, 185);

    //setAxisAutoScale(QwtAxis::YRight);
    setAxisAutoScale(QwtAxis::YLeft);
    axisScaleEngine(QwtAxis::YRight)->setAttribute(QwtScaleEngine::Attribute::Inverted,false);

    setAutoReplot(true);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}

THDPlot::THDPlot(QWidget * parent):FrequencyPlot(parent){
  textResult = new QwtPlotTextLabel();
  textResult->attach(this);
}


static QString thdResultString(const ResultTHD & r){
  const ParamTHD & p = r.params;
  auto s = QString("THD+N@%1Hz = %2%\n"
                   "THD@%1Hz = %3%\n"
                   "SNR = %4db\n"
                   "(%5Hz ~ %6Hz)\n")
      .arg(p.frequency)
      .arg(QString::number(r.thdNoiseRate, 'f', 6) )
      .arg(QString::number(r.thdRate,'f',6))
      .arg(QString::number(r.snr,'f',2))
      .arg(p.freqMin).arg(p.freqMax);
  for(uint i = 0; i < r.harmonicsLevel.size(); i++){
      auto l = 20* log10(r.harmonicsLevel[i]);
        if(l > -150)
          s += QString("h%1  %2db\n").arg(i+1).arg(QString::number(l,'f',1));
    }
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

  pc->setSamples(f.data(), a.data(), f.size());
  QwtText t(thdResultString(r));
  t.setRenderFlags(Qt::AlignRight | Qt::AlignTop);
  QFont font;
  font.setBold( false );
  t.setFont(font);
  t.setColor(color);
  textResult->setText(t);

  auto s = r.harmonicSpectrum.getAmplitude20log10();
  //remove problems
  clean_spectrum(s);
  auto m = mean(s);
  auto d = stddev(s);
  //qDebug() << m << d;
  setAxisScale(QwtAxis::YLeft,std::max(-500.,m - d),r.harmonicSpectrum.getMaxAmplitude20log10()+6);

  //setAxisScale(QwtAxis::YLeft,std::max(-360.,r.harmonicSpectrum.getMinAmplitude20log10()), );

  setAutoReplot(true);
  replot();
}

