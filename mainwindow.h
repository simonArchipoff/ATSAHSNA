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



class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void setFaustDsp(QString code);
    void measure(Backend * b, ParamResponse p);
    void measure(Backend * b, ParamTHD p);
    void measure(Backend * b, ParamSpectrogram p);

    void measureResponse();
    void measureTHD();
private:
//    QScopedPointer<BodePlot> QResults;
    QScopedPointer<QBackends> backends;
    QScopedPointer<QResults> qResults;
};
