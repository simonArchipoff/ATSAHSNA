#include "harmonicsplot.h"



THDPlot::THDPlot(QWidget* parent):FrequencyPlot(parent){
}
void THDPlot::setResult(std::variant<const std::vector<ResultHarmonics>> & r){
    auto v = get<const std::vector<ResultHarmonics>>(r);
    for(uint i = 0; i < v.size(); i++){
        auto name = QString{v[i].name.data()};
        setPlot(v[i].harmonicSpectrum, name,false);
        plots[name]->phaseDisplayed = false;
        //phase_axis->hide();
    }
    replot();
}
