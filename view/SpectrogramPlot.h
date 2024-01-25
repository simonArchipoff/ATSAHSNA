#pragma once
#include <QWidget>
#include <qcustomplot.h>
#include "Spectrogram.h"
#include <Response.h>
#include <variant>
#include <mutex>

#include <QHBoxLayout>

class SpectrogramPlot : public QCustomPlot {
    Q_OBJECT
public:
    SpectrogramPlot(QWidget* parent = nullptr);
    ~SpectrogramPlot();

    void plotSpectrogram(const ResultSpectrogram& spectrogram);
    void setResult(const ResultResponse & r){
        auto t = r.response.frequencyDomainTotemporal();
#if 0
        auto spect = stft(t.data(), t.data() + t.size(), 1024, 2, v[0].response.getSampleRate());
#else

        auto spect = spectrogram(t
                                 ,6
                                 ,128
                                 ,r.response.getSampleRate());
#endif
        plotSpectrogram(spect);
    }

private:
    QCPColorMap * colorMap;
};



class SpectrogramPlots : public QWidget{
    Q_OBJECT

public:
    SpectrogramPlots(QWidget* parent = nullptr):QWidget(parent),m(),layout(new QHBoxLayout){
        setLayout(layout);
    }

    void setResult(const std::variant<const std::vector<ResultResponse>> & var){
        auto v = std::get<const std::vector<ResultResponse>>(var);
        for(auto const & i : v){
            QString n(i.name.c_str());
            if(!m.contains(n)){
                auto s = new SpectrogramPlot(this);
                m.insert(QString(i.name.c_str()),s);
                layout->addWidget(s);
            }
            m[n]->setResult(i);
        }
    }
private:
    QMap<QString, SpectrogramPlot *> m;
    QHBoxLayout * layout;
};
