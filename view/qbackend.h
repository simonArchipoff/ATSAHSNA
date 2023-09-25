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



class QBackendsView : public QTabWidget{
public:
    QBackendsView(QWidget * parents):QTabWidget(parents){
        //setTabsClosable(true);
    }
    QSharedPointer<QFaustView>  addFaust(QString name);
    QSharedPointer<QJackView>   addJack();

protected:
    QVector<QSharedPointer<QFaustView>> fausts;
    std::vector<QSharedPointer<QJackView>>  jacks;
};
