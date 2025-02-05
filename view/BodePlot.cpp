#include "BodePlot.h"

#include <cmath>

#include <QFormLayout>
#include <QCheckBox>

BodePlot::BodePlot(QWidget*parent):ResultBase(tr("Bode Plot"),parent),fp(new FrequencyPlot(parent)){
    addWidget(fp);
    auto m = new BodePlotMenu;
    setConfigureWidget(m);
    connect(m, &BodePlotMenu::displayPhase, fp, &FrequencyPlot::displayPhase);
}


void BodePlot::setResult(const ResultResponse & r, QString name, QColor c){
    fp->setPlot(r.response, name, c);
}


BodePlotMenu::BodePlotMenu(){
    auto * phase = new QCheckBox(tr("phase"),this);
    auto * layout = new QVBoxLayout(this);
    layout->addWidget(phase);
    phase->setChecked(true);
    connect(phase,&QCheckBox::stateChanged, this, &BodePlotMenu::displayPhase);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}


void clean_spectrum(VD &s){
  for(uint i = 0; i < s.size(); i++){
      if(std::isnan(s[i]) || std::isinf(s[i]) || s[i] < -400){
          if(i == 0){
              s[i] = s[i+1];
            } else {
              s[i] = s[i-1];
            }
        }
    }
}

/*
void  transform(QLineSeries *s, const VD & x,const VD & y){
    assert(x.size() == y.size());
    QList<QPointF> l;
    for(uint i = 1; i < x.size(); i+= 1 ){ //TODO : fix that with a proper algorithm
        l.push_back(QPointF(x[i],y[i]));
    }
    s->replace(l);
}
*/
void setMinMaxAmplitude(double & minA, double & maxA, const VD&a, const VD & f, const double & maxF){
    minA = 400;
    maxA = -400;
    for(uint i = 0; i < f.size(); i++){
        if(f[i] > maxF)
            return;
        if(a[i] < minA && std::isfinite(a[i])){
            minA = a[i];
        }
        if(a[i] > maxA && std::isfinite(a[i])){
            maxA = a[i];
        }
    }
}





/*
BodeCurve::BodeCurve(const QColor c) : color(c) {
    c_phase.reset(new QwtPlotCurve());
    c_phase->setRenderHint(QwtPlotItem::RenderAntialiased);
    c_phase->setPen(c,1,Qt::DotLine);
    c_phase->setYAxis(QwtAxis::YRight);

    c_amplitude.reset(new QwtPlotCurve());
    c_amplitude->setRenderHint(QwtPlotItem::RenderAntialiased);
    c_amplitude->setPen(c,1,Qt::SolidLine);
    c_amplitude->setStyle(QwtPlotCurve::Lines);
    c_amplitude->setYAxis(QwtAxis::YLeft);

    displayed=true;
};

void BodeCurve::attach(QwtPlot * p){
    c_phase->attach(p);
    c_amplitude->attach(p);
}



void BodePlot::setResult(const FDF &c, const QColor color,double freqMin, double freqMax){
    setAutoReplot(false);
    QString colname = color.name();
    if(!curves.contains(colname))
      curves.insert(colname, new BodeCurve(color));
    BodeCurve * bc = curves[colname];
    bc->attach(this);
    bc->setCurve(c,freqMin,freqMax);

    //QwtPlot::setAxisScale(QwtAxis::XBottom,std::max<double>(10,bc->minFrequency()),bc->maxFrequency());
    QwtPlot::setAxisScale(QwtAxis::XBottom,bc->minFrequency,bc->maxFrequency);
    QwtPlot::setAxisScale(QwtAxis::YLeft,bc->minAmplitude,bc->maxAmplitude);
    setAutoReplot(true);
    replot();
}


uint find_first_greater_than(vector<double> v, double e){
  for(uint i = 0; i < v.size(); i++){
      if(v[i] >= e)
        return i;
    }
  return 0;
}
uint find_last_smaller_than(vector<double> v, double e){
  for(uint i = 0 ; i < v.size(); i++){
      auto idx = v.size() - 1 - i;
      if(v[idx] > e)
        return idx;
    }
  return v.size() -1;
}

void BodeCurve::setCurve(const FDF &c, double minF, double maxF){
    //auto r = c.getDecimatedAmplitude20log10PhaseFrequency(c.getFrequency().size());
    FDFLOG l{c,10};
    l.trimHF(maxF);
    l.trimLF(minF);
    vector<double> a = l.getAmplitude20log10();
      //get<0>(r);
    vector<double> p = l.getPhase();
      //get<1>(r);
    vector<double> f = l.getFrequency();
      //  get<2>(r);
    //p = try_make_phase_continuous(p);
    //p = decimation_log(p,10000);
    //a = decimation_log(a,10000);
    //vector<double> f = c.getFrequency();

    const int s = f.size();
    //f = decimation_log(f,10000);

    for(uint i = 0; i < a.size(); i++){
        if(a[i] < -100)
          p[i]=NAN;
      }

  c_amplitude->setSamples(f.data(), a.data(),s);
  c_phase->setSamples(f.data(),p.data(),s);

  minFrequency = f[1];
  maxFrequency = f[f.size() - 1];
  int min = find_first_greater_than(f,minF);
  int max = find_last_smaller_than(f,maxF);
  assert(min < max);

  maxAmplitude = *std::max_element(a.begin() + min, a.begin() + max);
  minAmplitude = *std::min_element(a.begin() + min, a.begin() + max);
}



BodePlot::BodePlot(QWidget* parent) : FrequencyPlot{parent}
{
    setAutoReplot(false);

    setAxisVisible(QwtAxis::YRight);
    setAxisTitle(QwtAxis::YLeft, tr("amplitude"));
    setAxisTitle(QwtAxis::YRight, tr("phase"));

    QwtPlot::setAxisScale(QwtAxis::YLeft,-50,6);
    setAxisScale(QwtAxis::YRight, -185, 185);

    //setAxisAutoScale(QwtAxis::YRight);
    setAxisAutoScale(QwtAxis::YLeft);
    axisScaleEngine(QwtAxis::YRight)->setAttribute(QwtScaleEngine::Attribute::Inverted,false);

    setAutoReplot(true);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}

THDPlot::THDPlot(QWidget * parent):FrequencyPlot(parent){
  textResult = new QwtPlotTextLabel();
  textResult->attach(this);
}


static QString thdResultString(const ResultHarmonics & r){
  const ParamHarmonics & p = r.params;
  auto s = QString("THD+N@%1Hz = %2%\n"
                   "THD@%1Hz = %3%\n"
                   "SNR = %4db\n"
                   "(%5Hz ~ %6Hz)\n")
      .arg(p.frequency)
      .arg(QString::number(r.thdNoiseRate, 'f', 6) )
      .arg(QString::number(r.thdRate,'f',6))
      .arg(QString::number(r.snr,'f',2))
      .arg(p.freqMin).arg(p.freqMax);
  for(uint i = 0; i < r.harmonicsLevel.size(); i++){
      auto l = 20* log10(r.harmonicsLevel[i]);
        if(l > -150)
          s += QString("h%1  %2db\n").arg(i+1).arg(QString::number(l,'f',1));
    }
  return s;
}


void THDPlot::setResult(const ResultHarmonics &r, QColor color){
  QString colname = color.name();
  if(!curves.contains(colname))
    curves.insert(colname, new QwtPlotCurve());
  QwtPlotCurve * pc = curves[colname];

  pc->setRenderHint(QwtPlotItem::RenderAntialiased);
  pc->setPen(color,1,Qt::SolidLine);
  pc->setStyle(QwtPlotCurve::Lines);
  pc->setYAxis(QwtAxis::YLeft);

  pc->attach(this);

  auto a = r.harmonicSpectrum.getAmplitude20log10();
  auto f = r.harmonicSpectrum.getFrequency();

  pc->setSamples(f.data(), a.data(), f.size());
  QwtText t(thdResultString(r));
  t.setRenderFlags(Qt::AlignRight | Qt::AlignTop);
  QFont font;
  font.setBold( false );
  t.setFont(font);
  t.setColor(color);
  textResult->setText(t);

  auto s = r.harmonicSpectrum.getAmplitude20log10();
  //remove problems
  clean_spectrum(s);
  auto m = mean(s);
  auto d = stddev(s);
  //qDebug() << m << d;
  setAxisScale(QwtAxis::YLeft,std::max(-500.,m - d),r.harmonicSpectrum.getMaxAmplitude20log10()+6);

  //setAxisScale(QwtAxis::YLeft,std::max(-360.,r.harmonicSpectrum.getMinAmplitude20log10()), );

  setAutoReplot(true);
  replot();
}
*/

/*

void qwtThingsSetFrequencyLogAxis(QwtPlot * plot,QwtAxisId axis){
    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->enableYMin(false);
    grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
    grid->setMinorPen(Qt::gray,  0, Qt::DotLine);
    grid->attach(plot);
    auto log_scale = new QwtLogScaleEngine(10);
    QwtScaleDiv xDiv = log_scale->divideScale(10.0, 1000.0, 2, 10, 1.0);
    plot->setAxisScaleEngine(axis, log_scale);
    plot->setAxisMaxMajor(axis, 6);
    plot->setAxisMaxMinor(axis, 9);
}

FrequencyPlot::FrequencyPlot(QWidget * parent) : QwtPlot{parent}
{
    QwtPlotCanvas * canvas = new QwtPlotCanvas();
    setCanvas(canvas);

    // grid

    qwtThingsSetFrequencyLogAxis(this,QwtAxis::XBottom);


    // axes
    setAxisScale(QwtAxis::XBottom,20,20000);
    setAxisScale(QwtAxis::YLeft,-50,5);
    setAxisScale(QwtAxis::YRight,180,-180);
    setAxisTitle(QwtAxis::XBottom, tr("frequency"));
    setAxisTitle(QwtAxis::YRight,tr("phase"));

    auto p =  new QwtPlotPanner(canvas);
    p->setAxisEnabled(QwtAxis::YRight,false);
}


// the data comes from this website : https://www.kennethmoreland.com/color-advice/
// thank you very much Kenneth Moreland
extern unsigned char viridis_rgb[3*256];
extern unsigned char black_body_rgb[3*256];

ColorMapViridis::ColorMapViridis()
    : QwtLinearColorMap()//Qt::darkBlue, Qt::darkRed)
{
    setFormat((QwtColorMap::Format) QwtColorMap::RGB);
    const unsigned int nb_color = sizeof(viridis_rgb) / 3;
    for(uint i = 0; i < nb_color; i++){
        addColorStop(static_cast<double>(i)/static_cast<double>(nb_color)
                     ,QColor(viridis_rgb[3*i],viridis_rgb[3*i+1],viridis_rgb[3*i+2]));
    }
    setMode(ScaledColors);
}


ColorMapBlackBody::ColorMapBlackBody()
    : QwtLinearColorMap()//Qt::darkBlue, Qt::darkRed)
{
    setFormat((QwtColorMap::Format) QwtColorMap::RGB);
    const unsigned int nb_color = sizeof(viridis_rgb) / 3;
    for(uint i = 0; i < nb_color; i++){
        addColorStop(static_cast<double>(i)/static_cast<double>(nb_color)
                     ,QColor( black_body_rgb[3*i]
                            ,black_body_rgb[3*i+1]
                            ,black_body_rgb[3*i+2]));
    }
    setMode(ScaledColors);
}




unsigned char viridis_rgb[256*3] =
    {68,1,84
     ,68,2,86
     ,69,4,87
     ,69,5,89
     ,70,7,90
     ,70,8,92
     ,70,10,93
     ,70,11,94
     ,71,13,96
     ,71,14,97
     ,71,16,99
     ,71,17,100
     ,71,19,101
     ,72,20,103
     ,72,22,104
     ,72,23,105
     ,72,24,106
     ,72,26,108
     ,72,27,109
     ,72,28,110
     ,72,29,111
     ,72,31,112
     ,72,32,113
     ,72,33,115
     ,72,35,116
     ,72,36,117
     ,72,37,118
     ,72,38,119
     ,72,40,120
     ,72,41,121
     ,71,42,122
     ,71,44,122
     ,71,45,123
     ,71,46,124
     ,71,47,125
     ,70,48,126
     ,70,50,126
     ,70,51,127
     ,70,52,128
     ,69,53,129
     ,69,55,129
     ,69,56,130
     ,68,57,131
     ,68,58,131
     ,68,59,132
     ,67,61,132
     ,67,62,133
     ,66,63,133
     ,66,64,134
     ,66,65,134
     ,65,66,135
     ,65,68,135
     ,64,69,136
     ,64,70,136
     ,63,71,136
     ,63,72,137
     ,62,73,137
     ,62,74,137
     ,62,76,138
     ,61,77,138
     ,61,78,138
     ,60,79,138
     ,60,80,139
     ,59,81,139
     ,59,82,139
     ,58,83,139
     ,58,84,140
     ,57,85,140
     ,57,86,140
     ,56,88,140
     ,56,89,140
     ,55,90,140
     ,55,91,141
     ,54,92,141
     ,54,93,141
     ,53,94,141
     ,53,95,141
     ,52,96,141
     ,52,97,141
     ,51,98,141
     ,51,99,141
     ,50,100,142
     ,50,101,142
     ,49,102,142
     ,49,103,142
     ,49,104,142
     ,48,105,142
     ,48,106,142
     ,47,107,142
     ,47,108,142
     ,46,109,142
     ,46,110,142
     ,46,111,142
     ,45,112,142
     ,45,113,142
     ,44,113,142
     ,44,114,142
     ,44,115,142
     ,43,116,142
     ,43,117,142
     ,42,118,142
     ,42,119,142
     ,42,120,142
     ,41,121,142
     ,41,122,142
     ,41,123,142
     ,40,124,142
     ,40,125,142
     ,39,126,142
     ,39,127,142
     ,39,128,142
     ,38,129,142
     ,38,130,142
     ,38,130,142
     ,37,131,142
     ,37,132,142
     ,37,133,142
     ,36,134,142
     ,36,135,142
     ,35,136,142
     ,35,137,142
     ,35,138,141
     ,34,139,141
     ,34,140,141
     ,34,141,141
     ,33,142,141
     ,33,143,141
     ,33,144,141
     ,33,145,140
     ,32,146,140
     ,32,146,140
     ,32,147,140
     ,31,148,140
     ,31,149,139
     ,31,150,139
     ,31,151,139
     ,31,152,139
     ,31,153,138
     ,31,154,138
     ,30,155,138
     ,30,156,137
     ,30,157,137
     ,31,158,137
     ,31,159,136
     ,31,160,136
     ,31,161,136
     ,31,161,135
     ,31,162,135
     ,32,163,134
     ,32,164,134
     ,33,165,133
     ,33,166,133
     ,34,167,133
     ,34,168,132
     ,35,169,131
     ,36,170,131
     ,37,171,130
     ,37,172,130
     ,38,173,129
     ,39,173,129
     ,40,174,128
     ,41,175,127
     ,42,176,127
     ,44,177,126
     ,45,178,125
     ,46,179,124
     ,47,180,124
     ,49,181,123
     ,50,182,122
     ,52,182,121
     ,53,183,121
     ,55,184,120
     ,56,185,119
     ,58,186,118
     ,59,187,117
     ,61,188,116
     ,63,188,115
     ,64,189,114
     ,66,190,113
     ,68,191,112
     ,70,192,111
     ,72,193,110
     ,74,193,109
     ,76,194,108
     ,78,195,107
     ,80,196,106
     ,82,197,105
     ,84,197,104
     ,86,198,103
     ,88,199,101
     ,90,200,100
     ,92,200,99
     ,94,201,98
     ,96,202,96
     ,99,203,95
     ,101,203,94
     ,103,204,92
     ,105,205,91
     ,108,205,90
     ,110,206,88
     ,112,207,87
     ,115,208,86
     ,117,208,84
     ,119,209,83
     ,122,209,81
     ,124,210,80
     ,127,211,78
     ,129,211,77
     ,132,212,75
     ,134,213,73
     ,137,213,72
     ,139,214,70
     ,142,214,69
     ,144,215,67
     ,147,215,65
     ,149,216,64
     ,152,216,62
     ,155,217,60
     ,157,217,59
     ,160,218,57
     ,162,218,55
     ,165,219,54
     ,168,219,52
     ,170,220,50
     ,173,220,48
     ,176,221,47
     ,178,221,45
     ,181,222,43
     ,184,222,41
     ,186,222,40
     ,189,223,38
     ,192,223,37
     ,194,223,35
     ,197,224,33
     ,200,224,32
     ,202,225,31
     ,205,225,29
     ,208,225,28
     ,210,226,27
     ,213,226,26
     ,216,226,25
     ,218,227,25
     ,221,227,24
     ,223,227,24
     ,226,228,24
     ,229,228,25
     ,231,228,25
     ,234,229,26
     ,236,229,27
     ,239,229,28
     ,241,229,29
     ,244,230,30
     ,246,230,32
     ,248,230,33
     ,251,231,35
     ,253,231,37};

unsigned char black_body_rgb[3*256] =
    {0,0,0
     ,3,1,1
     ,7,2,1
     ,10,3,2
     ,13,4,2
     ,16,5,3
     ,18,6,3
     ,20,7,4
     ,22,8,4
     ,24,9,5
     ,26,10,5
     ,27,11,6
     ,29,11,6
     ,30,12,7
     ,32,13,8
     ,33,14,8
     ,34,15,9
     ,36,15,9
     ,37,16,10
     ,38,16,10
     ,40,17,11
     ,41,17,11
     ,43,18,12
     ,44,18,12
     ,46,18,13
     ,47,19,13
     ,49,19,14
     ,50,19,14
     ,52,20,15
     ,54,20,15
     ,55,20,15
     ,57,21,16
     ,58,21,16
     ,60,21,16
     ,62,22,17
     ,63,22,17
     ,65,22,17
     ,66,23,18
     ,68,23,18
     ,70,23,18
     ,71,24,19
     ,73,24,19
     ,75,24,19
     ,76,25,20
     ,78,25,20
     ,80,25,20
     ,81,25,20
     ,83,26,21
     ,85,26,21
     ,86,26,21
     ,88,26,21
     ,90,27,22
     ,91,27,22
     ,93,27,22
     ,95,27,22
     ,97,28,23
     ,98,28,23
     ,100,28,23
     ,102,28,23
     ,104,29,24
     ,105,29,24
     ,107,29,24
     ,109,29,24
     ,111,29,25
     ,112,30,25
     ,114,30,25
     ,116,30,25
     ,118,30,26
     ,119,30,26
     ,121,31,26
     ,123,31,26
     ,125,31,27
     ,127,31,27
     ,128,31,27
     ,130,31,27
     ,132,32,28
     ,134,32,28
     ,136,32,28
     ,137,32,28
     ,139,32,29
     ,141,32,29
     ,143,32,29
     ,145,33,29
     ,147,33,30
     ,148,33,30
     ,150,33,30
     ,152,33,31
     ,154,33,31
     ,156,33,31
     ,158,33,31
     ,160,33,32
     ,161,34,32
     ,163,34,32
     ,165,34,32
     ,167,34,33
     ,169,34,33
     ,171,34,33
     ,173,34,33
     ,175,34,34
     ,177,34,34
     ,178,34,34
     ,179,36,34
     ,180,38,34
     ,181,40,33
     ,182,42,33
     ,183,44,33
     ,184,45,33
     ,185,47,32
     ,186,49,32
     ,187,50,32
     ,188,52,31
     ,189,53,31
     ,190,55,31
     ,191,56,31
     ,192,58,30
     ,193,59,30
     ,194,61,30
     ,195,62,29
     ,196,64,29
     ,197,65,28
     ,198,66,28
     ,199,68,28
     ,200,69,27
     ,201,71,27
     ,202,72,26
     ,203,73,26
     ,204,75,25
     ,205,76,25
     ,206,77,24
     ,207,79,24
     ,208,80,23
     ,209,82,23
     ,210,83,22
     ,211,84,21
     ,212,85,21
     ,213,87,20
     ,214,88,19
     ,215,89,19
     ,216,91,18
     ,217,92,17
     ,218,93,16
     ,219,95,15
     ,220,96,14
     ,221,97,13
     ,222,98,12
     ,223,100,11
     ,224,101,9
     ,225,102,8
     ,226,104,7
     ,227,105,5
     ,227,107,5
     ,227,109,6
     ,228,110,7
     ,228,112,7
     ,228,114,8
     ,228,116,8
     ,229,118,9
     ,229,119,10
     ,229,121,10
     ,229,123,11
     ,229,124,12
     ,230,126,12
     ,230,128,13
     ,230,130,14
     ,230,131,14
     ,230,133,15
     ,230,135,15
     ,231,136,16
     ,231,138,17
     ,231,140,17
     ,231,141,18
     ,231,143,19
     ,231,145,19
     ,231,146,20
     ,232,148,21
     ,232,150,21
     ,232,151,22
     ,232,153,22
     ,232,154,23
     ,232,156,24
     ,232,158,24
     ,232,159,25
     ,232,161,26
     ,232,162,26
     ,233,164,27
     ,233,166,27
     ,233,167,28
     ,233,169,29
     ,233,170,29
     ,233,172,30
     ,233,174,30
     ,233,175,31
     ,233,177,32
     ,233,178,32
     ,233,180,33
     ,233,181,34
     ,233,183,34
     ,233,185,35
     ,233,186,35
     ,233,188,36
     ,233,189,37
     ,233,191,37
     ,233,192,38
     ,233,194,38
     ,233,195,39
     ,233,197,40
     ,233,199,40
     ,233,200,41
     ,232,202,42
     ,232,203,42
     ,232,205,43
     ,232,206,43
     ,232,208,44
     ,232,209,45
     ,232,211,45
     ,232,213,46
     ,232,214,47
     ,232,216,47
     ,231,217,48
     ,231,219,48
     ,231,220,49
     ,231,222,50
     ,231,223,50
     ,231,225,51
     ,230,226,52
     ,230,228,52
     ,230,229,53
     ,231,231,60
     ,233,231,69
     ,234,232,78
     ,236,233,87
     ,237,234,94
     ,238,235,102
     ,240,236,109
     ,241,236,117
     ,242,237,124
     ,243,238,131
     ,245,239,137
     ,246,240,144
     ,247,241,151
     ,248,241,158
     ,249,242,164
     ,249,243,171
     ,250,244,177
     ,251,245,184
     ,252,246,190
     ,252,247,197
     ,253,248,203
     ,253,249,210
     ,254,249,216
     ,254,250,223
     ,254,251,229
     ,255,252,236
     ,255,253,242
     ,255,254,249
     ,255,255,255};
*/


