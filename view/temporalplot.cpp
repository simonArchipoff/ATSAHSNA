#include "temporalplot.h"




template <typename T, int N>
T ecart_type(T* begin) {
    // Calcul de la somme
    T somme = 0;
    for (int i = 0; i < N; ++i) {
        somme += begin[i];
    }

    // Calcul de la moyenne
    T moyenne = somme / N;

    // Calcul de l'écart-type
    T variance = 0;
    for (int i = 0; i < N; ++i) {
        variance += std::pow(begin[i] - moyenne, 2);
    }
    T ecart_type = std::sqrt(variance / N);

    return  ecart_type;
}

template<typename T, int N>
std::vector<T> block_ecart_type(QVector<T> & t){
    std::vector<T> res;
    res.resize(t.size() / N);
    for(int i = 0; i < res.size(); i++){
        res[i] = ecart_type<T,N>(t.data() + i*N);
    }
    return res;
}



void PlotTemporal::setCurve(const FDF&f){
    response=f;
}

void PlotTemporal::plotData(){
    auto a = response.frequencyDomainTotemporal();
    QVector<double> qa(a.begin(),a.end());
    if(type==Step){
        double s = 0.0;
        for(int i = 0; i < qa.size(); i++){
            s += qa[i];
            qa[i] = s;
        }
    }
    QVector<double> qt(a.size());
    for(int i = 0; i < qt.size(); i++){
        qt[i] = (1.0*i) / response.getSampleRate();
        qa[i] /= (qt.size()) ;
    }
    const int N = 256;
    auto r = block_ecart_type<double,N>(qa);
    double max = *std::max_element(r.begin(),r.end());
    int tMin = -1;
    int tMax = 0;
    for(int i = 0; i < r.size(); i++){
        if(tMin== -1){
            if(r[i] > max/100){
                tMin=i*N;
            }
        }
        if(r[i] > max/100){
            tMax = i*N;
        }
    }
    timeRange.first = (tMin+0.0)/response.getSampleRate();
    timeRange.second = ((tMax+N) - 1.0)/response.getSampleRate();

    amplitudeRange.first = *std::min_element(qa.begin() + tMin , qa.begin() + tMax + N - 1);
    amplitudeRange.second = *std::max_element(qa.begin() + tMin , qa.begin() + tMax + N - 1);

    values->setData(qt,qa,true);
}



TemporalPlot::TemporalPlot(QWidget * parent):ResultBase(tr("temporal plot"),parent),plot(new MyQCustomPlot(parent)){
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    temporalAxis = plot->xAxis;
    amplitudeAxis = plot->yAxis;
    setConfigureWidget(menu = new TemporalPlotMenu());
    connect(menu,&TemporalPlotMenu::responseTypeChanged, this, &TemporalPlot::changeResponseType );
    temporalAxis->setLabel(tr("time (s)"));
    temporalAxis->setRange(0,0.1);
    amplitudeAxis->setLabel(tr("Amplitude"));
    amplitudeAxis->setRange(-1,1);
    addWidget(plot);
}
TemporalPlot::~TemporalPlot(){
    for(auto i : plots){
        delete i;
    }
}




void TemporalPlot::setPlot(const FDF & f, QString name, QColor color){
    if(!plots.contains(name)){
        auto pa = plot->addGraph(temporalAxis, amplitudeAxis);

        QPen pen_amplitude;

        pen_amplitude.setColor(color);
        pen_amplitude.setWidth(2);

        pen_amplitude.setStyle(Qt::SolidLine);
        pa->setPen(pen_amplitude);

        plots.insert(name,new PlotTemporal(color, pa, menu->responsetype));
    }
    plots[name]->setCurve(f);
    plots[name]->plotData();
}


void TemporalPlot::updatePlot(const FDF&v , QString name){
    auto * p = plots[name];
    assert(p);
    p->setCurve(v);
    p->plotData();
}

void TemporalPlot::replot(){
    double tMin,tMax,aMin,aMax;
    tMin = aMin = std::numeric_limits<double>::max();
    tMax = aMax = std::numeric_limits<double>::min();
    bool b = false;
    for(auto it = plots.cbegin(); it != plots.cend(); ++it){
        b = true;
        auto a = it.value()->amplitudeRange;
        auto t = it.value()->timeRange;
        if(t.first < tMin){
            tMin = t.first;
        }
        if(t.second > tMax){
            tMax=t.second;
        }

        if(a.first < aMin){
            aMin = a.first;
        }
        if(a.second > aMax){
            aMax=a.second;
        }

    }
    if(b){
        temporalAxis->setRange(tMin,tMax);
        amplitudeAxis->setRange(aMin,aMax);
    }


    plot->replot();
}

void TemporalPlot::removeResult(QString name){
    auto r = plots[name];
    plots.remove(name);
    if(r){
        plot->removePlottable(r->values);
        plot->replot();
        delete r;
    }
}

void TemporalPlot::changeResponseType(PlotTemporal::ResponseType t){
    for (auto it = plots.cbegin(); it != plots.cend(); ++it) {
        it.value()->type = t;
        it.value()->plotData();
    }
    plot->replot();
}

TemporalPlotMenu::TemporalPlotMenu():QWidget(){
    auto* layout = new QVBoxLayout(this);

    //auto* label = new QLabel(tr("response type:"), this);
    //layout->addWidget(label);

    auto * comboBox = new QComboBox(this);
    comboBox->addItem("Impulse",PlotTemporal::Impulse);
    comboBox->addItem("Step",PlotTemporal::Step);
    layout->addWidget(comboBox);

    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this,comboBox](int index) {
        auto newType = comboBox->currentData().value<PlotTemporal::ResponseType>();
        if(newType != responsetype){
            responsetype = newType;
            emit responseTypeChanged(newType);
        }
    });

    setLayout(layout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
