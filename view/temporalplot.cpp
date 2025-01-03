#include "temporalplot.h"

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
    values->setData(qt,qa,true);
    values->rescaleAxes(false);
}



TemporalPlot::TemporalPlot(QWidget * parent):ResultBase(tr("temporal plot"),parent),plot(new MyQCustomPlot(parent)){
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    temporalAxis = plot->xAxis;
    amplitudeAxis = plot->yAxis;
    setConfigureWidget(menu = new TemporalPlotMenu());
    connect(menu,&TemporalPlotMenu::responseTypeChanged, this, &TemporalPlot::changeResponseType );
    temporalAxis->setLabel(tr("time (s)"));
    amplitudeAxis->setLabel(tr("Amplitude"));
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
