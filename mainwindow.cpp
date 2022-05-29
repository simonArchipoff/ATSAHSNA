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
#include "task.h"
#include "view/qbackend.h"

#include "backend/measure.h"

#include <QCoroFuture>


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

  QSplitter * s = new QSplitter{this};
  auto l = new QVBoxLayout;

  l->addWidget(faustThings->viewFaust.data());
  l->addWidget(jackThings->viewJack.data());
  auto w = new QWidget;
  w->setLayout(l);

  w->setMaximumWidth(w->minimumSizeHint().width());


  setCentralWidget(s);
  //setLayout(hb);

  s->addWidget(w);
  //s->addWidget(measures.data());
  s->addWidget(qResults.data());
  //bodePlot->show();
  s->setStretchFactor(1,0);
  connect(qResults.data(), &QResults::request_response, this, [this](auto p,auto b){this->measure(getBackend(b),p);});
  connect(qResults.data(), &QResults::request_distortion, this,[this](auto p,auto b){this->measure(getBackend(b),p);});
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

QCoro::Task<void> MainWindow::measure(Backend * b, ParamResponse p){
  auto r = co_await QtConcurrent::run(compute_response,b,p);
  co_return this->qResults->setResult(r[0],Qt::red);
}

QCoro::Task<void> MainWindow::measure(Backend * b,ParamTHD p){
 auto r = co_await QtConcurrent::run(compute_distortion,b,p);
 co_return this->qResults->setResult(r[0],Qt::red);
}

#if 0
  s->setSizes({1 * faustThings->viewFaust->sizeHint().width(),
               1 * measures->sizeHint().width(),
               5 * bodePlot->sizeHint().width()});
#endif

 /* connect(measures.data(),&QMeasure::start_measure,this,[this](double freqMin){
      Backend * bf = this->faustThings->backendFaustQt.data();
      if(bf->isReady()){
          vector<double> in{impulse(freqMin, bf->getSampleRate())};
          vector<vector<double>> v({in});
          for(uint i = 1; i < bf->numberInput(); i++){
              v.push_back(vector<double>(in.size(),0));
            }
          bf->requestMeasure(v);
          std::optional<vector<vector<double>>> r = bf->tryGetOutput();
          if(r.has_value()){
              FDF f{compute_TF_FFT(in, r.value()[0] ,bf->getSampleRate())};
              this->bodePlot->setCurve(Qt::blue,f);
            }
        }
    });*/


  /*
  QTimer::singleShot(5000,this, [this](){
        vector<double> in = impulse(10,44100);
        auto out = in;
        out[0] = 0;
        out[10] = 1;
        auto f = compute_TF_FFT(in,out,44100);
        this->bodePlot->addCurve(f,Qt::blue);
        qDebug() << "foo";
   });
*/
  //QDockWidget * measureDock = new QDockWidget(tr("Measure"), this);

  //;

  //faustThings.data();

  /*
    measureDock->setWidget(qMeasure.data());
    measureDock->setAllowedAreas(Qt::TopDockWidgetArea);
    measureDock->setFeatures( QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::TopDockWidgetArea,measureDock);

     */
  //addDockWidget(Qt::LeftDockWidgetArea,backendDock);



  //this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

  //resizeDocks({backendDock},{500},Qt::Horizontal);
  //ui->bode_plot->addCurve(compute_TF_FFT(i,o,44000),Qt::GlobalColor::red);
  //foo.compute_1_1(i,o);
  /*
    //foo.getUI()->run();  */

  // connect(dsp_backend_ui, &qtFaustDsp::faustDspCode,
  //         this,&MainWindow::setFaustDsp);
  // connect(this->qMeasure, &QMeasure::start_measure,this,&MainWindow::measure);

  //dumpObjectTree();
//}

/*
void MainWindow::measureResponse(){

}

void MainWindow::measureTHD(){

}
*/


