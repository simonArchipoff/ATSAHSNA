#include "responseview.h"
#include "qmeasuresview.h"
#include <QVBoxLayout>

QResponseView::QResponseView(QWidget * parent):QWidget(parent),
    bodePlot(new BodePlot(this))
    ,temporalPlot(new TemporalPlot(this))
    ,spectrogramPlot(new SpectrogramPlots(this))
    ,measures(new QMeasuresView(this)){
    //setLayout(hLayout);
    QHBoxLayout * hLayout = new QHBoxLayout(this);
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(bodePlot);
    vLayout->addWidget(temporalPlot);
    vLayout->addWidget(spectrogramPlot);
    vLayout->setSizeConstraint(QLayout::SetMaximumSize);
    hLayout->addWidget(measures);
    hLayout->addLayout(vLayout);
    connect(measures,&QMeasuresView::remove,this,&QResponseView::removeResult);
}

/*
void QResponseView::setResult(std::variant<const std::vector<ResultResponse>>&r){
    std::vector<ResultResponse> v = std::get<const std::vector<ResultResponse>>(r);
    for(uint i = 0; i < v.size(); i++){

}
*/

bool QResponseView::isBodeInit(){
    return bodePlot;
}

bool QResponseView::isSpectrogramInit(){
    return spectrogramPlot;
}

void QResponseView::setResults(std::variant<const std::vector<ResultResponse> > &r){
    std::vector<ResultResponse> v = std::get<const std::vector<ResultResponse>>(r);
    for(uint i = 0; i < v.size(); i++){
        setResult(v[i]);
    }
    temporalPlot->replot();
    bodePlot->replot();
}



void QResponseView::setResult(const ResultResponse &r){
    auto m = measures->addResult(r);
    if(m){
        temporalPlot->setPlot(m->getResult().response, m->getName(), m->getColor());
        bodePlot->setPlot(m->getResult().response, m->getName(), m->getColor());
        spectrogramPlot->setResult(m->getResult(), m->getName());
        connect(m, &QResultResponseView::remove,this,&QResponseView::removeResult);
    } else {
        temporalPlot->updatePlot(m->getResult().response,m->getName());
        bodePlot->updatePlot(m->getResult().response,m->getName());
        spectrogramPlot->setResult(m->getResult(),m->getName());
    }


}

void QResponseView::removeResult(QString name){
    temporalPlot->removeResult(name);
    bodePlot->removeResult(name);
    spectrogramPlot->removeResult(name);
    this->measures->removeResult(name);

}


BodePlot * QResponseView::getBodePlot(){
    if(!bodePlot){
        bodePlot = new BodePlot(this);
    }
    return bodePlot;
}


SpectrogramPlots * QResponseView::getSpectrogramPlot(){
    if(!spectrogramPlot){
        spectrogramPlot = new SpectrogramPlots(this);
    }
    return spectrogramPlot;
}
