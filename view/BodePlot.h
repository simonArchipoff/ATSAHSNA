#pragma once




#include <QTabWidget>
#include <frequencyplot.h>

#include <QColor>
#include <QVector>

#include <qcustomplot.h>

#include <Response.h>
#include <Harmonics.h>

#include <helper_results_view.h>


class BodePlot : public ResultBase{
    Q_OBJECT
public:
    BodePlot(QWidget*parent);

    //void setResult(std::variant<const std::vector<ResultResponse>> & r);
    void setResult(const ResultResponse &r, QString name, QColor c);
    void setPlot(const FDF & f, QString name, QColor c, bool phaseDisp=true){
        fp->setPlot(f,name,c,phaseDisp);
    }
    void replot(){
        fp->replot();
    }

    void removeResult(QString name){
        fp->removeResult(name);
    }
    void updatePlot(const FDF&v,QString name){
        fp->updatePlot(v,name);
    }
    void updatePlot(const ResultResponse & r, QString name){
        updatePlot(r.response, name);
    }



protected:
    FrequencyPlot * fp;
};


class BodePlotMenu : public QWidget{
    Q_OBJECT
public:
    BodePlotMenu();

signals:
    void displayPhase(bool b);
};
