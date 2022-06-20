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

#include "backend/measure.h"



MainWindow::MainWindow(QWidget *parent)
  :QMainWindow{parent}
  ,faustThings{new FaustThings{this}}
  ,jackThings{new JackThings{this}}
  ,qResults{new QResults{this}}
  //,measures{new QMeasure{this}}
{
  //QToolBar * toolbar = addToolBar("main toolbar");
  //QAction * start_measure = toolbar->addAction("mesure");
  //connect(start_measure, &QAction::triggered, this, &MainWindow::measure);
          //qDebug()<< QThread::currentThreadId();

  auto s = new QSplitter{this};
  auto t = new QTabWidget;
  t->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  t->addTab(faustThings->viewFaust.data(),"Faust");
  t->addTab(jackThings->viewJack.data(),"Jackd");

  //w->setMaximumWidth(w->minimumSizeHint().width());

  //auto l = new QVBoxLayout;
  s->addWidget(t);
  setCentralWidget(s);
  //setLayout(hb);

  //s->addWidget(measures.data());
  s->addWidget(qResults.data());
  //bodePlot->show();
  //s->setSizes({1,10});
  connect(qResults.data(), &QResults::request_response, this, [this](auto p,auto b){this->measure(getBackend(b),p);});
  connect(qResults.data(), &QResults::request_distortion, this,[this](auto p,auto b){this->measure(getBackend(b),p);});
  connect(qResults.data(), &QResults::request_spectrogram,this,[this](auto p, auto b){this->measure(getBackend(b),p);});
  connect(jackThings->viewJack.data(), &QBackendJack::newLatency,this,[this](int i){ jackThings->backendJack->setLatency(i);});

  faustThings->viewFaust->compile();
}


Backend * MainWindow::getBackend(backend_type b){
  if(b == backend_type::FAUST)
    return faustThings->backendFaustQt.data();
  if(b == backend_type::JACK)
    return jackThings->backendJack.data();
  assert(false);
  exit(1);
}

void MainWindow::measure(Backend * b, ParamResponse p){
  auto r = QtConcurrent::run(compute_response,b,p);
  QtConcurrent::run([r,this](){this->qResults->setResult(r.result()[0],Qt::red);});
}

void MainWindow::measure(Backend * b,ParamTHD p){
 auto r = QtConcurrent::run(compute_distortion,b,p);
 QtConcurrent::run([r,this](){this->qResults->setResult(r.result()[0],Qt::red);});
 //this->qResults->setResult(r[0],Qt::red);
}

void MainWindow::measure(Backend * b, ParamSpectrogram p){
  auto r = QtConcurrent::run(compute_spectrogram,b,p);
  QtConcurrent::run([r,this](){this->qResults->setResult(r.result()[0],Qt::red);});
}
