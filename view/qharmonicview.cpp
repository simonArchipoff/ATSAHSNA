#include "qharmonicview.h"

THDPlot::THDPlot(QWidget* parent):FrequencyPlot(parent){
    displayPhase(false);
}

void THDPlot::setResult(const ResultHarmonics & r,QString name, QColor c){
        //auto name = QString{r.name.data()};
        setPlot(r.harmonicSpectrum, name, c);
        plots[name]->phaseDisplayed = false;
}


/*
void THDPlot::setResult(const std::vector<ResultHarmonics> &v){
        for(auto & r : v){
            setResult(r);
        }
        replot();
}*/



THDText::THDText(QWidget * parent):QWidget(parent){
}



QHarmonicView::QHarmonicView(QWidget * parent)
    :QWidget(parent)
    ,plot(new THDPlot(this))
    ,text(new THDText(this))
    ,measures(new QMeasuresView(this)){
    QHBoxLayout * hLayout = new QHBoxLayout;
    QVBoxLayout* vLayout = new QVBoxLayout;

    setLayout(hLayout);
    vLayout->addWidget(plot);
    vLayout->addWidget(text);
    hLayout->addWidget(measures);
    hLayout->addLayout(vLayout);
    vLayout->setSizeConstraint(QLayout::SetMaximumSize);
    connect(measures,&QMeasuresView::remove, this, & QHarmonicView::removeResult);
}

void QHarmonicView::setResults(const std::variant<const std::vector<ResultHarmonics>, std::monostate> &r){
    std::vector<ResultHarmonics> v = std::get<const std::vector<ResultHarmonics>>(r);
    for(uint i = 0; i < v.size(); i++){
        setResult(v[i]);
    }
    plot->replot();
}

void QHarmonicView::setResult(const ResultHarmonics &r){
    auto m = measures->addResult(r);
    if(m){
        plot->setResult(m->getResult(), m->getName(), m->getColor());
        //bodePlot->setPlot(m->getResult().response, m->getName(), m->getColor());
        //spectrogramPlot->setResult(m->getResult(), m->getName());
        connect(m, &QResultHarmonicsView::remove,this,&QHarmonicView::removeResult);
    } else {
        plot->updatePlot(m->getResult(),m->getName());
        //temporalPlot->updatePlot(m->getResult().response,m->getName());
        //bodePlot->updatePlot(m->getResult().response,m->getName());
        //spectrogramPlot->setResult(m->getResult(),m->getName());
    }
   // plot->setResult(r);
}

void QHarmonicView::removeResult(QString name){
    plot->removeResult(name);
    this->measures->removeResult(name);

}
