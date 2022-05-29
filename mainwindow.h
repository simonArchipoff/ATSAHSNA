#pragma once

#include <QMainWindow>
#include <QScopedPointer>

#include "constants.h"

#include "view/qbackend.h"
#include "view/BodePlot.h"
#include "view/qmeasure.h"
#include "view/qresults.h"
#include "backend/backendFaust.h"
#include "backend/backendJack.h"
#include <QCoroFuture>


/*
class JackThings : public QObject {
  Q_OBJECT
public:
  JackThings(QWidget * parent=nullptr):viewJack{new JackView},backendJack{new BackendJack}{
  }


  QScopedPointer<JackView> viewJack;
  QScopedPointer<BackendJack> backendJack;
};
*/

class FaustThings : public QObject {
  Q_OBJECT
public:
  FaustThings(QWidget * parent)
    :viewFaust(new QFaustDsp{parent}),
     backendFaustQt{new BackendFaustQT{parent}}{
     connect(viewFaust.data(), &QFaustDsp::faustDspCode,
             this,&FaustThings::setFaustCode);
  }

  void setFaustCode(QString code,uint sampleRate){
      auto res = create_faust_qt(code,sampleRate,viewFaust.data());
      try{
          auto tmp = std::get<BackendFaustQT*>(res);
          backendFaustQt.reset(tmp);
          viewFaust->setErrorMessage("");
          viewFaust->setUI(backendFaustQt->getUI());
      } catch (const std::bad_variant_access& ex) {
          auto s = std::get<QString>(res);
          viewFaust->setErrorMessage(s);
      }
  }
  QScopedPointer<QFaustDsp> viewFaust;
  QScopedPointer<BackendFaustQT> backendFaustQt;
};

class JackThings : public QObject{
  Q_OBJECT
public:
  JackThings(QWidget * parent)
    :backendJack{new BackendJack}
    ,viewJack{new QBackendJack}
  {
    connect(viewJack.data(),&QBackendJack::newInputPort, this,[this](){backendJack->addInputPort();});
    connect(viewJack.data(),&QBackendJack::newOutputPort,this,[this](){backendJack->addOutputPort();});
  }

  QScopedPointer<BackendJack> backendJack;
  QScopedPointer<QBackendJack> viewJack;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void setFaustDsp(QString code);
    QCoro::Task<void> measure(Backend * b, ParamResponse p);
    QCoro::Task<void> measure(Backend * b, ParamTHD p);

    void measureResponse();
    void measureTHD();
    Backend * getBackend(backend_type b);
private:
//    QScopedPointer<BodePlot> QResults;
    QScopedPointer<FaustThings> faustThings;
    QScopedPointer<JackThings> jackThings;
    QScopedPointer<QResults> qResults;
};
