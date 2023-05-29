#include "TemporalPlot.h"
#include "qboxlayout.h"
#include "qnamespace.h"
#include "qwt_axis.h"
#include <QVBoxLayout>
#include <QFormLayout>

TemporalPlot::TemporalPlot(QWidget * parent)
  : QWidget{parent}
  , start{new QSlider{Qt::Horizontal,this}}
  , len{new QSlider{Qt::Horizontal, this}}
  , input_data{}
  , output_data{}
  , plot{new QwtPlot{this}}
  , input_curve{new QwtPlotCurve{}}
  , output_curve{new QwtPlotCurve{}}
{
  input_curve->attach(plot.data());
  output_curve->attach(plot.data());

  QFormLayout * form = new QFormLayout();
  setLayout(form);
  form->addWidget(plot.data());
  form->addRow(tr("start"),start.data());
  form->addRow(tr("size"),len.data());

  plot->setAxisAutoScale(QwtAxis::YLeft);
  plot->setAxisAutoScale(QwtAxis::YRight);
  plot->enableAxis(QwtAxis::YRight);
  connect(start.data(),&QSlider::valueChanged, this,[this](auto i){
      plot->setAxisScale(QwtAxis::XBottom, i, i+ len->value());
    });
  connect(len.data(), &QSlider::valueChanged, this, [this](auto l){
      plot->setAxisScale(QwtAxis::XBottom, start->value(), start->value()+l);
    });
  plot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}


void TemporalPlot::setInput(const VD &in){
  plot->setAutoReplot(false);
  input_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
  input_curve->setPen(Qt::blue,2,Qt::SolidLine);
  input_curve->setStyle(QwtPlotCurve::Dots);
  input_curve->setYAxis(QwtAxis::YLeft);
    input_curve->setSamples(in.data(), in.size());
  plot->setAutoReplot(true);
  plot->replot();
}
void TemporalPlot::setOutput(const VD &out){
  plot->setAutoReplot(false);
  output_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
  output_curve->setPen(Qt::red,2,Qt::SolidLine);
  output_curve->setStyle(QwtPlotCurve::Dots);
  output_curve->setYAxis(QwtAxis::YRight);
  output_curve->setSamples(out.data(), out.size());
  plot->setAutoReplot(true);
  plot->replot();
  start->setRange(1,out.size());
  len->setRange(1,10000);
  len->setValue(1000);//out.size());
}




