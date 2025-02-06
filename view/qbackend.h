#pragma once

#include "qlineedit.h"
#include "qwidget.h"
#include <QWidget>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>

#if ENABLE_JACK
#include <Jack.h>
#include <QJackView.h>
#endif

#include <Faust.h>
#include <faust/gui/QTUI.h>

#include <qsoundfilewidget.h>


class QFaustView : public QWidget{
    Q_OBJECT
public:
    explicit QFaustView(QWidget *parent = nullptr);

    void setDSPUI(QWidget * ui);
    void setErrorMessage(QString);
    void compile();

    void setFile(QString path);

signals:
    void setFaustCode(QString code, uint sampleRate);
    void setHarmonic(bool);
    void setResponse(bool);

public:
    QWidget * dspUi;
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
    QFaustView *  addFaust(QString name);
#if ENABLE_JACK
    QJackView  * addJack();
    std::vector<QJackView *>  jacks;
#endif

    QSoundFileWidget *addSoundFile();

protected:
    QVector<QFaustView *> fausts;
    QVector<QSoundFileWidget *> soundfiles;

};
