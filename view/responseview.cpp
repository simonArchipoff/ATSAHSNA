#include "responseview.h"
#include <QVBoxLayout>

QResponseView::QResponseView(QWidget * parents):
    bodePlot(new BodePlot(this))
    ,temporalPlot(new TemporalPlot(this))
    ,spectrogramPlot(new SpectrogramPlots(this)){

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(bodePlot);
    mainLayout->addWidget(temporalPlot);
    mainLayout->addWidget(spectrogramPlot);
    setLayout(mainLayout);
}

void QResponseView::setResult(std::variant<const std::vector<ResultResponse>>&r){
    std::vector<ResultResponse> v = std::get<const std::vector<ResultResponse>>(r);
    for(uint i = 0; i < v.size(); i++){
        temporalPlot->setPlot(v[i].response,QString{v[i].name.data()});
    }
    temporalPlot->replot();
}

bool QResponseView::isBodeInit(){
    return bodePlot;
}

bool QResponseView::isSpectrogramInit(){
    return spectrogramPlot;
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
