#pragma once

#include <QMainWindow>
#include <QScopedPointer>


#include "qbackend.h"

#include <signalHarmonics.h>
#include <signalResponse.h>
#include <signalSpectrogram.h>
#include <BodePlot.h>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

    QSharedPointer<QFaustDsp> faust;
    BodePlot * bode;
};
