#include "qmeasuresview.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>


QResultView::QResultView(QColor color, QString name, QWidget *parent)
    :QWidget(parent)
    ,layout(new QVBoxLayout(this))
    ,color(color)
    ,name(name)
    ,colorFrame(new QFrame(this)){

    setLayout(layout);
    auto closeButton = new QPushButton(tr("close"), this);
    auto copyButton = new QPushButton(tr("copy"),this);
    auto temporal = new QCheckBox(tr("temporal"), this);
    auto spectrum = new QCheckBox(tr("spectrum"), this);

    // Configuration du layout
    layout->addWidget(colorFrame);
    layout->addWidget(closeButton);
    layout->addWidget(copyButton);
    layout->addWidget(temporal);
    layout->addWidget(spectrum);

    colorFrame->setFrameShape(QFrame::Box);
    colorFrame->setAutoFillBackground(true);
    QPalette palette = colorFrame->palette();
    palette.setColor(QPalette::Window, color);
    colorFrame->setPalette(palette);

    // Connexions des signaux et slots
    connect(closeButton, &QPushButton::clicked, this, &QResultView::remove);
    connect(copyButton, &QPushButton::clicked, this, [this](){emit copy(this->name);});
    connect(temporal, &QCheckBox::toggled, this, &QResultView::setTemporalDisplay);
    connect(spectrum, &QCheckBox::toggled, this, &QResultView::setFrequencyDisplay);
}

void QResultView::setFrequencyDisplay(bool newFrequency_display){
    frequency_display = newFrequency_display;
    emit frequencyDisplay(name, frequency_display);
}

void QResultView::setTemporalDisplay(bool newTemporal_display){
    temporal_display = newTemporal_display;
    emit temporalDisplay(name,temporal_display);
}

QString QResultView::getName() const{
    return name;
}



QResultHarmonicsView::QResultHarmonicsView(QColor color, const ResultHarmonics &r, QWidget *parent)
    : QResultView(color,QString(r.name.c_str()),parent)
    ,result(r){
    auto text = new QCheckBox(tr("text"), this);
    layout->addWidget(text);

}

void QResultHarmonicsView::setSpectrogramDisplay(bool newSpectrogram_display){
    spectrogram_display = newSpectrogram_display;
    emit spectrogramDisplay(name,newSpectrogram_display);
}

QResultResponseView::QResultResponseView(QColor color, const ResultResponse &r, QWidget *parent)
    : QResultView(color,QString(r.name.c_str()),parent)
    ,result(r){
    auto spectrum = new QCheckBox(tr("spectrogram"), this);
    layout->addWidget(spectrum);
}

void QResultResponseView::setTextDisplay(bool newText_display){
    text_display = newText_display;
    emit textDisplay(name,newText_display);
}

QMeasuresView::QMeasuresView():layout(new QVBoxLayout(this)){
    setLayout(layout);
}

void QMeasuresView::removeResult(QResultView * w){
    layout->removeWidget(w);
    emit remove(w->getName());
    delete w;
}
