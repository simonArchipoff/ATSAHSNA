#include "qharmonicview.h"

THDPlot::THDPlot(QWidget* parent):FrequencyPlot(parent){
}

void THDPlot::setResult(const ResultHarmonics & r){
        auto name = QString{r.name.data()};
        setPlot(r.harmonicSpectrum, name,false);
        plots[name]->phaseDisplayed = false;
        //phase_axis->hide();
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

void QHarmonicView::setResult(const std::variant<const std::vector<ResultHarmonics>> &r){
}
