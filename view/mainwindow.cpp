#include "mainwindow.h"

#include <signal.h>
#include <QDebug>
#include <QScopedPointer>
#include <QToolBar>
#include <QVBoxLayout>

#include "qnamespace.h"
//#include "task.h"
#include "qbackend.h"
#include <QChartView>
#include <QtConcurrent/QtConcurrent>



MainWindow::MainWindow(QWidget *parent)
    :QMainWindow{parent}
    ,backends{new QBackends(this)}
    ,displays{new QDisplays{this}}
  //,measures{new QMeasure{this}}
{
  //QToolBar * toolbar = addToolBar("main toolbar");
  //QAction * start_measure = toolbar->addAction("mesure");
  //connect(start_measure, &QAction::triggered, this, &MainWindow::measure);
          //qDebug()<< QThread::currentThreadId();

  auto s = new QSplitter{this};

  setCentralWidget(s);
  s->addWidget(backends.data());
  s->addWidget(displays.data());
//  backends->setWidth(backends->minimumSizeHint().width());
  //auto l = new QVBoxLayout;
  //s->addWidget(backends.data());

  s->show();

  //setLayout(hb);

  //auto b = backends->addFaustBackend();
  //backends->addJackBackend();

  /*connect(b,&BackendFaustQT::changed,this,[this](){

    });
*/
}
/*
void MainWindow::measure(Backend * b, ParamResponse p){
  auto r = compute_response(b,p);
  if(r.size() > 0)
    this->qResults->setResult(r[0],Qt::red);
  if(r.size() > 1)
    this->qResults->setResult(r[1],Qt::blue);
  if(r.size() > 2)
    this->qResults->setResult(r[2],Qt::green);
}

void MainWindow::measure(Backend * b,ParamHarmonics p){
  auto r = compute_distortion(b,p);
  if(r.size() > 0)
    this->qResults->setResult(r[0],Qt::red);
  if(r.size() > 1)
    this->qResults->setResult(r[1],Qt::blue);
  if(r.size() > 2)
    this->qResults->setResult(r[2],Qt::green);
}

void MainWindow::measure(Backend * b, ParamSpectrogram p){
  //auto r = compute_spectrogram(b,p);
  //if(r.size() > 0)
  //  this->qResults->setResult(r[0],Qt::red);
 // if(r.size() > 1)
 //   this->qResults->setResult(r[1],Qt::blue);
 // if(r.size() > 2)
 //   this->qResults->setResult(r[2],Qt::green);
}
*/