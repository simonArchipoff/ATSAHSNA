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
    ,backends{new QBackendsView(this)}
    ,displays{new QDisplays{this}}
  //,measures{new QMeasure{this}}
{
  //QToolBar * toolbar = addToolBar("main toolbar");
  //QAction * start_measure = toolbar->addAction("mesure");
  //connect(start_measure, &QAction::triggered, this, &MainWindow::measure);
          //qDebug()<< QThread::currentThreadId();

  auto s = new QSplitter{this};

  setCentralWidget(s);
  s->addWidget(backends);

  s->addWidget(displays);
  s->setStretchFactor(0,1);
  s->setStretchFactor(1,4);
  // setWidth(backends->minimumSizeHint().width());
  //auto l = new QVBoxLayout;
  //s->addWidget(backends.data());
  createMenus();
  s->show();

  paramResponseWidget = new ParamResponseWidget(this);
  paramHarmonicsWidget = new ParamHarmonicsWidget(this);
  spectrogramWidget = new ParamSpectrogramWidget(this);


  // Configuration initiale : cacher les widgets
  paramResponseWidget->hide();
  paramHarmonicsWidget->hide();
  spectrogramWidget->hide();


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



// chat gpt :




MainWindow::~MainWindow()
{
}

void MainWindow::createMenus()
{
  QMenu* backendMenu = new QMenu("Backend", this);
  QAction* addFaustAction = backendMenu->addAction("Add Faust");
  QAction* addJackAction = backendMenu->addAction("Add Jack");
  menuBar()->addMenu(backendMenu);

  QMenu* analyseMenu = new QMenu("Analyse", this);
  QAction* addResponseAction = analyseMenu->addAction("Add Response");
  QAction* addHarmonicsAction = analyseMenu->addAction("Add Harmonics");
  QAction* addSpectrogramAction = analyseMenu->addAction("Add Spectrogram");
  menuBar()->addMenu(analyseMenu);

  // Connecter les signaux des actions aux slots appropriés
  connect(addResponseAction, &QAction::triggered, this, &MainWindow::onAddResponseWidgetRequested);
  connect(addHarmonicsAction, &QAction::triggered, this, &MainWindow::onAddHarmonicsWidgetRequested);
  connect(addSpectrogramAction, &QAction::triggered, this, &MainWindow::onAddSpectrogramWidgetRequested);

  // Connecter les signaux personnalisés aux slots appropriés
  connect(this, &MainWindow::addResponseWidgetRequested, this, &MainWindow::onAddResponseWidgetRequested);
  connect(this, &MainWindow::addHarmonicsWidgetRequested, this, &MainWindow::onAddHarmonicsWidgetRequested);
  connect(this, &MainWindow::addSpectrogramWidgetRequested, this, &MainWindow::onAddSpectrogramWidgetRequested);
  connect(addFaustAction, &QAction::triggered, this, &MainWindow::onAddFaustBackendRequested);
  connect(addJackAction, &QAction::triggered, this, &MainWindow::onAddJackBackendRequested);
}

void MainWindow::onAddResponseWidgetRequested()
{
  paramResponseWidget->show();
}

void MainWindow::onAddHarmonicsWidgetRequested()
{
  paramHarmonicsWidget->show();
}

void MainWindow::onAddSpectrogramWidgetRequested()
{
  spectrogramWidget->show();
}

void MainWindow::onAddFaustBackendRequested()
{
  // Logique pour ajouter le backend Faust
  // Émettre le signal pour indiquer que le backend a été ajouté
  emit addFaustBackendRequested();
}

void MainWindow::onAddJackBackendRequested()
{
  // Logique pour ajouter le backend Jack
  // Émettre le signal pour indiquer que le backend a été ajouté
  emit addJackBackendRequested();
}


