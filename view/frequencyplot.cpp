#include "frequencyplot.h"




PlotAmplitudePhase::PlotAmplitudePhase(QString name, QColor c,QCPGraph * amplitude, QCPGraph * phase):amplitude(amplitude),phase(phase),color(c),name(name){
}



double perpendicularDistance(double x, double y, double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double d = std::sqrt(dx * dx + dy * dy);

    return std::abs(dy * x - dx * y + x2 * y1 - y2 * x1) / d;
}

// dodgy implementation of douglas peucker algorithm I made with my coding friend chat-gpt
// it intend to reduce the number of points while preserving the shape of the curve. It doesn't work well
void simplifyCurve(const std::vector<double> &x, const std::vector<double> &y, const vector<double>&y1, double epsilon,
                   std::vector<double> &xOut, std::vector<double> &yOut, std::vector<double> &y1Out) {
    if (x.size() < 2 || y.size() < 2) {
        return;
    }

    double maxDistance = 0.0;
    int index = 0;
    for (size_t i = 1; i < x.size() - 1; ++i) {
        double distance = perpendicularDistance(x[i], y[i], x.front(), y.front(), x.back(), y.back());
        if(x[i] > - 100)
            distance += perpendicularDistance(x[i], y1[i],x.front(),y1.front(), x.back(),y1.back());
        if (distance > maxDistance) {
            index = i;
            maxDistance = distance;
        }
    }

    if (maxDistance > epsilon) {
        std::vector<double> xLeft, yLeft, xRight, yRight;
        std::vector<double> xResult1, yResult1,y1Result1, xResult2, yResult2,y1Result2;


        simplifyCurve({x.begin(), x.begin() + index + 1}, {y.begin(), y.begin() + index + 1},{y1.begin(), y1.begin() + index+1}, epsilon, xResult1, yResult1,y1Result1);
        simplifyCurve({x.begin() + index, x.end()}, {y.begin() + index, y.end()},{y1.begin() + index, y1.end()}, epsilon, xResult2, yResult2, y1Result2);


        xResult1.pop_back();
        yResult1.pop_back();
        y1Result1.pop_back();
        xOut.insert(xOut.end(), xResult1.begin(), xResult1.end());
        yOut.insert(yOut.end(), yResult1.begin(), yResult1.end());
        y1Out.insert(y1Out.end(), y1Result1.begin(), y1Result1.end());
        xOut.insert(xOut.end(), xResult2.begin(), xResult2.end());
        yOut.insert(yOut.end(), yResult2.begin(), yResult2.end());
        y1Out.insert(y1Out.end(), y1Result2.begin(), y1Result2.end());
    } else {
        xOut.push_back(x.front());
        yOut.push_back(y.front());
        y1Out.push_back(y1.front());
        xOut.push_back(x.back());
        yOut.push_back(y.back());
        y1Out.push_back(y1.back());
    }
}
void PlotAmplitudePhase::setCurve(const FDF&f){
    auto fr = f.getFrequency();
    auto fa = f.getAmplitude20log10();
    auto fp = f.getPhase();

    const int n = 1048;

    if(fr.size() < n){
        setCurve(fr,
                 fa,
                 fp);
        return;
    }
    VD frd, fad,fpd;
#if 0

    const double s = 1.l/n;
    for(uint i = 0; i < n; i++){
        const double d = (exp(i * s) - 1) / (M_E - 1)  ;
        const uint idx = d * fr.size();
        //assert(idx >= 0);
        assert(idx < fr.size());

        frd[i] = fr[idx];
        fad[i] = fa[idx];
        fpd[i] = fp[idx];
    }
#endif


    fr.erase(fr.begin());
    fa.erase(fa.begin());
    fp.erase(fp.begin());


    simplifyCurve(fr,fa,fp,0.1,frd, fad, fpd);

    setCurve(frd,
             fad,
             fpd);
}




void PlotAmplitudePhase::setCurve(const VD&f, const VD&a, const VD&p){
    auto qf = QVector<double>(f.begin(), f.end());
    auto qa = QVector<double>(a.begin(), a.end());
    auto qp = QVector<double>(p.begin(), p.end());
    amplitude->setData(qf,qa,true);
    phase->setData(qf,qp,true);
    amplitude->rescaleAxes(true);
}

FrequencyPlot::FrequencyPlot(QWidget * parent):MyQCustomPlot(parent){
    frequencyAxis = xAxis;
    amplitudeAxis = yAxis;
    phaseAxis = yAxis2;
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    frequencyAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog>  logTicker(new QCPAxisTickerLog);
    frequencyAxis->setTicker(logTicker);
    frequencyAxis->setLabel(tr("Frequency (Hz)"));
    frequencyAxis->setRange(20,20000);
    amplitudeAxis->setLabel(tr("Amplitude"));

    phaseAxis->setLabel(tr("Phase (degree)"));
    phaseAxis->setRange(-180,180);
}
FrequencyPlot::~FrequencyPlot(){
    for(auto i : plots){
        delete i;
    }
}

void FrequencyPlot::setPlot(const FDF & f, QString name, QColor color){
    if(!plots.contains(name)){
        auto pa = addGraph(frequencyAxis, amplitudeAxis);
        auto pp = addGraph(frequencyAxis, phaseAxis);

        QPen pen_phase,pen_amplitude;
        pen_phase.setColor(color);
        pen_phase.setWidth(2);
        pen_amplitude = pen_phase;
        pen_phase.setStyle(Qt::DotLine);
        pen_amplitude.setStyle(Qt::SolidLine);
        phaseAxis->setVisible(display_phase);
        pa->setPen(pen_amplitude);
        pp->setPen(pen_phase);

        plots.insert(name,new PlotAmplitudePhase(name, color,pa,pp));
    }
    plots[name]->setDisplayPhase(display_phase);
    plots[name]->setCurve(f);
    phaseAxis->setVisible(display_phase);
}

void FrequencyPlot::removeResult(QString name){
    auto r = plots[name];
    this->removePlottable(r->amplitude);
    this->removePlottable(r->phase);
    replot();
    plots.remove(name);
    delete r;
}

void FrequencyPlot::updatePlot( const FDF&v, QString name){
    auto * p = plots[name];
    assert(p);
    p->setCurve(v);
}

void FrequencyPlot::displayPhase(bool b){
    display_phase = b;
    for(auto & i : plots){
        i->setDisplayPhase(b);
    }
    replot();
}
