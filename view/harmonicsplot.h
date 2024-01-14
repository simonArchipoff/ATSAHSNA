#pragma once

#include "frequencyplot.h"
#include "Harmonics.h"

class THDPlot : public FrequencyPlot
{
public:
    THDPlot(QWidget* parent);
    void setResult(std::variant<const std::vector<ResultHarmonics>> & r);
};
