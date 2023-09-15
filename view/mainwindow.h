#pragma once

#include <QMainWindow>
#include <QScopedPointer>


#include "qbackend.h"
#include <widget_chatgpt.h>

#include <Harmonics.h>
#include <Response.h>
#include <Spectrogram.h>
#include <BodePlot.h>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSharedPointer<QBackends> backends;
    QSharedPointer<QDisplays> displays;

private:
    void createMenus();
public:
    void onAddResponseWidgetRequested();
    void onAddHarmonicsWidgetRequested();
    void onAddSpectrogramWidgetRequested();
    void onAddFaustBackendRequested();
    void onAddJackBackendRequested();
public:
signals:
    void addResponseWidgetRequested();
    void addHarmonicsWidgetRequested();
    void addSpectrogramWidgetRequested();
    void addFaustBackendRequested();
    void addJackBackendRequested();



private:
    ParamResponseWidget* paramResponseWidget;
    ParamHarmonicsWidget* paramHarmonicsWidget;
    ParamSpectrogramWidget* spectrogramWidget;
};


