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
  createMenus();
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



// chat gpt :
void MainWindow::createMenus()
{
  // Créer le menu "Backend"
  QMenu *backendMenu = new QMenu("Backend");
  QAction *addFaustAction = backendMenu->addAction("Add Faust");
  QAction *addJackAction = backendMenu->addAction("Add Jack");

  // Connecter les actions du menu "Backend" à des slots (si besoin)
  connect(addFaustAction, &QAction::triggered, this, &MainWindow::onAddFaust);
  connect(addJackAction, &QAction::triggered, this, &MainWindow::onAddJack);

  // Créer le menu "Analyse"
  QMenu *analyseMenu = new QMenu("Analyse");
  QAction *addResponseAction = analyseMenu->addAction("Add Response");
  QAction *addHarmonicsAction = analyseMenu->addAction("Add Harmonics");
  QAction *addSpectrogramAction = analyseMenu->addAction("Add Spectrogram");

  // Connecter les actions du menu "Analyse" à des slots (si besoin)
  connect(addResponseAction, &QAction::triggered, this, &MainWindow::onAddResponse);
  connect(addHarmonicsAction, &QAction::triggered, this, &MainWindow::onAddHarmonics);
  connect(addSpectrogramAction, &QAction::triggered, this, &MainWindow::onAddSpectrogram);

  // Ajouter les menus à la barre de menu de la fenêtre
  menuBar()->addMenu(backendMenu);
  menuBar()->addMenu(analyseMenu);
}

// Implémenter les slots ici pour effectuer les actions spécifiques
void MainWindow::onAddFaust() { /* Action pour ajouter Faust */ }
void MainWindow::onAddJack() { /* Action pour ajouter Jack */ }
void MainWindow::onAddResponse() { /* Action pour ajouter Response */ }
void MainWindow::onAddHarmonics() { /* Action pour ajouter Harmonics */ }
void MainWindow::onAddSpectrogram() { /* Action pour ajouter Spectrogram */}
