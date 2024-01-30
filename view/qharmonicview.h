#pragma once

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include "frequencyplot.h"
#include "Harmonics.h"

class THDPlot : public FrequencyPlot
{
public:
    THDPlot(QWidget* parent);
    void setResult(const ResultHarmonics & r);
    void setResult(const std::vector<ResultHarmonics> &);
};


class THDText : public QWidget {
public:
    THDText(QWidget * parent=nullptr);

protected:
    //QLayout * layout_text;
    //QMap<QString, QTextEdit *> m;
};



class QHarmonicView : public QWidget {
    Q_OBJECT
public:
    QHarmonicView(QWidget * parent=nullptr);

    void setResult(const std::variant<const std::vector<ResultHarmonics>> &);
    void setResult(const ResultHarmonics &);
protected:
    THDPlot * plot;
    THDText * text;

};

