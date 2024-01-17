#include "responseview.h"
#include <QVBoxLayout>

QDisplays::QDisplays(QWidget * parents):
    bodePlot(new BodePlot(this))
    ,temporalPlot(new TemporalPlot(this))
    ,spectrogramPlot(new SpectrogramPlots(this)){

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(bodePlot);
    mainLayout->addWidget(temporalPlot);
    mainLayout->addWidget(spectrogramPlot);
    setLayout(mainLayout);
}

void QDisplays::setResult(std::variant<const std::vector<ResultResponse>>&r){
    std::vector<ResultResponse> v = get<const std::vector<ResultResponse>>(r);
    for(uint i = 0; i < v.size(); i++){
        temporalPlot->setPlot(v[i].response,QString{v[i].name.data()});
    }
    temporalPlot->replot();
}

bool QDisplays::isBodeInit(){
    return bodePlot;
}

bool QDisplays::isSpectrogramInit(){
    return spectrogramPlot;
}


BodePlot * QDisplays::getBodePlot(){
    if(!bodePlot){
        bodePlot = new BodePlot(this);
    }
    return bodePlot;
}


SpectrogramPlots * QDisplays::getSpectrogramPlot(){
    if(!spectrogramPlot){
        spectrogramPlot = new SpectrogramPlots(this);
    }
    return spectrogramPlot;
}
