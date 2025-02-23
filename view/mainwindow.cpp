#include "mainwindow.h"

#include <QDebug>
#include <QScopedPointer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QApplication>
#include <QPalette>
//#include "task.h"
#include "qbackend.h"

#include <QtConcurrent/QtConcurrent>





MainWindow::MainWindow(QWidget *parent)
    :QMainWindow{parent}
    ,backends{new QBackendsView(this)}
    ,response{new QResponseView{this}}
    ,harmonic{new QHarmonicView{this}}
    //,paramResponseWidget{nullptr}
    //,paramHarmonicsWidget{nullptr}
    //,paramSpectrogramWidget{nullptr}
    //,measures{new QMeasure{this}}
{
  //QToolBar * toolbar = addToolBar("main toolbar");
  //QAction * start_measure = toolbar->addAction("mesure");
  //connect(start_measure, &QAction::triggered, this, &MainWindow::measure);
          //qDebug()<< QThread::currentThreadId();

  auto s = new QSplitter{this};

  setCentralWidget(s);
  s->addWidget(backends);

  QTabWidget * tabs = new QTabWidget(this);
  tabs->addTab(response,tr("response"));
  tabs->addTab(harmonic,tr("harmonics"));
  s->addWidget(tabs);
  //s->setStretchFactor(0,1);
  //s->setStretchFactor(1,5);

  //int totalWidth = s->size().width();  // Taille totale du splitter
  s->setSizes({100,400});
  //setWidth(backends->minimumSizeHint().width());
  //auto l = new QVBoxLayout;
  //s->addWidget(backends.data());
  createMenus();
  s->show();

  //paramResponseWidget = new ParamResponseWidget(this);
  //paramHarmonicsWidget = new ParamHarmonicsWidget(this);
  //spectrogramWidget = new ParamSpectrogramWidget(this);


  // Configuration initiale : cacher les widgets
  //paramResponseWidget->hide();
  //paramHarmonicsWidget->hide();
  //spectrogramWidget->hide();


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




MainWindow::~MainWindow(){
    delete backends;
    delete response;
    delete harmonic;
}

void MainWindow::createMenus()
{
  QMenu* backendMenu = new QMenu("Backend", this);
  QAction * openFileAction = backendMenu->addAction("open file");
  connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);

  QAction* addFaustAction = backendMenu->addAction("Add Faust");
  connect(addFaustAction, &QAction::triggered, this, &MainWindow::onAddFaustBackendRequested);
#ifdef ENABLE_JACK
  QAction* addJackAction = backendMenu->addAction("Add Jack");
  connect(addJackAction, &QAction::triggered, this, &MainWindow::onAddJackBackendRequested);
#endif

  QAction * addSoundFileAction = backendMenu->addAction("Add Audio file");
  connect(addSoundFileAction, &QAction::triggered,this, &MainWindow::onAddSoundFileRequested);

  menuBar()->addMenu(backendMenu);

  QMenu* analyseMenu = new QMenu(tr("Response"), this);

  QAction* spectrum = analyseMenu->addAction("Spectrum");
  spectrum->setCheckable(true);

  connect(spectrum, &QAction::toggled, this, &MainWindow::onAddSpectrumWidgetRequested);

  QAction* temporal = analyseMenu->addAction("Temporal");
  temporal->setCheckable(true);
  connect(temporal, &QAction::toggled, this, &MainWindow::onAddTemporalWidgetRequested);


  QAction* spectrogram = analyseMenu->addAction("Spectrogram");
  spectrogram->setCheckable(true);
  connect(spectrogram, &QAction::toggled, this, &MainWindow::onAddSpectrogramWidgetRequested);

  menuBar()->addMenu(analyseMenu);
  spectrum->setChecked(true);  onAddSpectrumWidgetRequested(true);
  temporal->setChecked(false); onAddTemporalWidgetRequested(false);
  spectrogram->setChecked(false);  onAddSpectrogramWidgetRequested(false);

}

void MainWindow::onAddSpectrumWidgetRequested(bool b){
  response->setBodeView(b);
}

void MainWindow::onAddTemporalWidgetRequested(bool b){
  response->setTemporalView(b);
}

void MainWindow::onAddSpectrogramWidgetRequested(bool b){
  response->setSpectrogramView(b);
}



void MainWindow::onAddFaustBackendRequested(){
  emit addFaustBackendRequested();
}

void MainWindow::onAddJackBackendRequested(){
  emit addJackBackendRequested();
}

void MainWindow::onAddSoundFileRequested(){
    emit addSoundFileRequested();
}


bool isAudioFile(const QString &filePath) {
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filePath);
    return type.name().startsWith("audio/");
}



void MainWindow::openFile(){
  auto path = QFileDialog::getOpenFileName(this
                                        , tr("Open File")
                                        ,""
                                       ,"*");
  if(path.endsWith(".dsp")){
      emit addFaustBackendRequested(path);
  }else if(isAudioFile(path)){
      emit addSoundFileRequested(path);
  }
}


