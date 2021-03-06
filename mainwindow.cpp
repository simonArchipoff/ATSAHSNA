#include "mainwindow.h"

#include <signal.h>
#include <QDebug>
#include <QScopedPointer>
#include <QToolBar>
#include <QVBoxLayout>

#include "backend/backendJack.h"
#include "constants.h"

#include "backend/backend.h"
#include "backend/backendFaust.h"

#include "backend/acquisition.h"

#include "qnamespace.h"
//#include "task.h"
#include "view/qbackend.h"

#include <QtConcurrent/QtConcurrent>



MainWindow::MainWindow(QWidget *parent)
  :QMainWindow{parent}
  ,backends{new QBackends{this}}
  ,qResults{new QResults{this}}
  //,measures{new QMeasure{this}}
{
  //QToolBar * toolbar = addToolBar("main toolbar");
  //QAction * start_measure = toolbar->addAction("mesure");
  //connect(start_measure, &QAction::triggered, this, &MainWindow::measure);
          //qDebug()<< QThread::currentThreadId();

  auto s = new QSplitter{this};
  //w->setMaximumWidth(w->minimumSizeHint().width());

  //auto l = new QVBoxLayout;
  s->addWidget(backends.data());
  setCentralWidget(s);
  //setLayout(hb);

  //s->addWidget(measures.data());
  s->addWidget(qResults.data());
  //bodePlot->show();
  //s->setSizes({1,10});
  connect(qResults.data(), &QResults::request_response,   this,
          [this](auto p){this->measure(backends->getSelectedBackend(),p);});
  connect(qResults.data(), &QResults::request_distortion, this,
          [this](auto p){this->measure(backends->getSelectedBackend(),p);});
  connect(qResults.data(), &QResults::request_spectrogram,this,
          [this](auto p){this->measure(backends->getSelectedBackend(),p);});

  backends->addFaustBackend();
  backends->addJackBackend();
}

void MainWindow::measure(Backend * b, ParamResponse p){
  auto r = compute_response(b,p);
  if(r.size() > 0)
    this->qResults->setResult(r[0],Qt::red);
}

void MainWindow::measure(Backend * b,ParamTHD p){
  auto r = compute_distortion(b,p);
  if(r.size() > 0)
    this->qResults->setResult(r[0],Qt::red);
}

void MainWindow::measure(Backend * b, ParamSpectrogram p){
  auto r = compute_spectrogram(b,p);
  if(r.size() > 0)
    this->qResults->setResult(r[0],Qt::red);
}
