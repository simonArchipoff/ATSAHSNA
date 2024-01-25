#ifndef QMEASURESVIEW_H
#define QMEASURESVIEW_H

#include <QObject>
#include <QWidget>


#include <Harmonics.h>
#include <Response.h>

QWidget * measureToQWidget(QWidget * parent, const ResultHarmonics &);
QWidget * measureToQWidget(QWidget * parent, const ResultResponse &);


class QResultView : public QWidget {

public:
    QResultView( QColor color, QString name, QWidget * parent=nullptr):QWidget(parent),color(color),name(name){

    }
    QLayout * layout;
    QColor color;
    QString name;
};

class QResultHarmonicsView : public QResultView{

};

class QResponseResponseView : public QResultView{

};


class QMeasuresView : public QWidget
{
    Q_OBJECT
public:
    QMeasuresView();
};

#endif // QMEASURESVIEW_H
