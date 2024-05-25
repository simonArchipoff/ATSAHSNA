#include "qharmonicview.h"

THDPlot::THDPlot(QWidget* parent):FrequencyPlot(parent){
    displayPhase(false);
}

void THDPlot::setResult(const ResultHarmonics & r){
        auto name = QString{r.name.data()};
        setPlot(r.harmonicSpectrum, name,false);
        plots[name]->phaseDisplayed = false;
}

void THDPlot::setResult(const std::vector<ResultHarmonics> &v){
        for(auto & r : v){
            setResult(r);
        }
        replot();
}



THDText::THDText(QWidget * parent):QWidget(parent){
}



QHarmonicView::QHarmonicView(QWidget * parent)
    :QWidget(parent)
    ,plot(new THDPlot(this))
    ,text(new THDText(this)){
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(plot);
    mainLayout->addWidget(text);
    setLayout(mainLayout);
}

void QHarmonicView::setResults(const std::variant<const std::vector<ResultHarmonics>, std::monostate> &r){
    std::vector<ResultHarmonics> v = std::get<const std::vector<ResultHarmonics>>(r);
    for(uint i = 0; i < v.size(); i++){
        setResult(v[i]);
    }
    plot->replot();
}

void QHarmonicView::setResult(const ResultHarmonics &r){
    plot->setResult(r);
}
