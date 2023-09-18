#pragma once

#include "qlineedit.h"
#include <QSharedPointer>
#include "qwidget.h"
#include <QWidget>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>

#include <Jack.h>
#include <Faust.h>
#include <faust/gui/QTUI.h>

#include <QJackView.h>



class QFaustView : public QWidget{
    Q_OBJECT
public:
    explicit QFaustView(QWidget *parent = nullptr);

    void setDSPUI(QWidget * ui);
    void setErrorMessage(QString);
    void compile();
signals:
    void setFaustCode(QString code, uint sampleRate);

public:
    QSharedPointer<QWidget> dspUi;
    QLineEdit * sr;
    QTextEdit * codeEdit;
    QLabel * errorLabel;
    QPushButton * compile_button;
    QVBoxLayout * layout;
};








class QBackends : public QTabWidget{
public:
    QBackends(QWidget * parents):QTabWidget(parents){
    }
    QSharedPointer<QFaustView> addFaust();
    QSharedPointer<QJackView> addJack();
protected:
    std::vector<QSharedPointer<QFaustView>> fausts;
    std::vector<QSharedPointer<QJackView>> jacks;
};
