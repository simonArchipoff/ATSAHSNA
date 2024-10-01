#pragma once

#include <QObject>
#include <QWidget>

#include <QTableWidget>


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

class THDText : public QTableWidget {
public:
    THDText(QWidget * parent=nullptr);
    void setResult(const ResultHarmonics & r, QString name, QColor c);
    void removeResult(QString);

protected:
    const uint nb_harmonics = 20;
    QTableWidget * table;
    QMap<QString, int> m;
};



class QHarmonicView : public QWidget {
    Q_OBJECT
public:
    QHarmonicView(QWidget * parent=nullptr);

    void setResults(const std::variant<const std::vector<ResultHarmonics>, ErrorBackend> &);
    void setResult(const ResultHarmonics &);

    void removeResult(QString);

protected:
    THDPlot * plot;
    THDText * text;
    QMeasuresView * measures;
};

