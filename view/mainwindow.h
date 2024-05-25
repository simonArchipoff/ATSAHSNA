#pragma once

#include <QMainWindow>
#include <QScopedPointer>


#include "qbackend.h"
#include <widget_chatgpt.h>

#include <Harmonics.h>
#include <Response.h>
#include <Spectrogram.h>
#include <BodePlot.h>
#include <responseview.h>
#include <qharmonicview.h>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QBackendsView *backends;
    QResponseView *response;
    QHarmonicView *harmonic;
private:
    void createMenus();
public:
    void onAddSpectrumWidgetRequested(bool);
    void onAddTemporalWidgetRequested(bool);
    void onAddSpectrogramWidgetRequested(bool);


    void onAddFaustBackendRequested();
    void onAddJackBackendRequested();

    void openFile();

public:
signals:
    void addResponseWidgetRequested();
    void addHarmonicsWidgetRequested();
    void addSpectrogramWidgetRequested();

    void addFaustBackendRequested();
    void AddFaustBackendRequested(QString file);
    void addJackBackendRequested();
};


