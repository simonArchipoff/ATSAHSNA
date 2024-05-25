#pragma once

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include "frequencyplot.h"
#include "Harmonics.h"

#include <qmeasuresview.h>

class THDPlot : public FrequencyPlot
{
public:
    THDPlot(QWidget* parent);
    void setResult(const ResultHarmonics & r, QString name, QColor c);
    void setResult(const std::vector<ResultHarmonics> &);

    void updatePlot(const ResultHarmonics & r, QString name){
        FrequencyPlot::updatePlot(r.harmonicSpectrum,name);
    }
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

    void setResults(const std::variant<const std::vector<ResultHarmonics>, std::monostate> &);
    void setResult(const ResultHarmonics &);

    void removeResult(QString);

protected:
    THDPlot * plot;
    THDText * text;
    QMeasuresView * measures;
};

