#include "qmeasuresview.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QApplication>
#include <QStyle>


QResultView::QResultView(QColor color, QString name, QWidget *parent)
    :QWidget(parent)
    ,layout(new QHBoxLayout)
    ,color(color)
    ,name(name)
    ,colorFrame(new QFrame(this)){
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setLayout(layout);
    auto closeButton = new QPushButton(this);
    auto label = new QLabel(this);
    label->setText(name);
    //auto copyButton = new QPushButton(tr("copy"),this);
    //auto temporal = new QCheckBox(tr("temporal"), this);
    //auto spectrum = new QCheckBox(tr("spectrum"), this);

    QIcon closeIcon = QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton);
    closeButton->setIcon(closeIcon);
    closeButton->setIconSize(QSize(16, 16));

    // Configuration du layout
    layout->addWidget(colorFrame);
    layout->addWidget(label);
    layout->addWidget(closeButton);
    //layout->addWidget(copyButton);
    //layout->addWidget(temporal);
    //layout->addWidget(spectrum);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    colorFrame->setFrameShape(QFrame::Box);
    colorFrame->setAutoFillBackground(true);
    colorFrame->setFixedSize(16, 16);
    QPalette palette = colorFrame->palette();
    palette.setColor(QPalette::Window, color);
    colorFrame->setPalette(palette);

    // Connexions des signaux et slots
    connect(closeButton, &QPushButton::released, this, [this]() {
        emit QResultView::remove(getName());
    });
    //connect(copyButton,  &QPushButton::clicked, this, [this](){emit copy(this->name);});
    //connect(temporal,    &QCheckBox::toggled,   this, &QResultView::setTemporalDisplay);
    //connect(spectrum,    &QCheckBox::toggled,   this, &QResultView::setFrequencyDisplay);
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
    //auto text = new QCheckBox(tr("text"), this);
    //layout->addWidget(text);
}

void QResultHarmonicsView::setSpectrogramDisplay(bool newSpectrogram_display){
    spectrogram_display = newSpectrogram_display;
    emit spectrogramDisplay(name,newSpectrogram_display);
}

void QResultHarmonicsView::updateResult(const ResultHarmonics &r){
    result = r;
    emit updatedResult(r, getName());
}

QResultResponseView::QResultResponseView(QColor color, const ResultResponse &r, QWidget *parent)
    : QResultView(color,QString(r.name.c_str()),parent)
    ,result(r){
    //auto spectrum = new QCheckBox(tr("spectrogram"), this);
    //layout->addWidget(spectrum);
}

void QResultResponseView::setTextDisplay(bool newText_display){
    text_display = newText_display;
    emit textDisplay(name,newText_display);
}

void QResultResponseView::updateResult(const ResultResponse &r){
    result = r;
    emit updatedResult(r, getName());
}

QMeasuresView::QMeasuresView(QWidget * parent):QWidget(parent),layout(new QVBoxLayout(this)){
  //  auto l = new QVBoxLayout(this);
    layout->addStretch();
    setLayout(layout);

    //setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
}

void QMeasuresView::removeResult(QString name){
    {
        auto w = harmonicsMap[name];
        if(w){
            layout->removeWidget(w);
            harmonicsMap.remove(name);
            delete w;
        }
    }
    {
        auto w = responseMap[name];
        if(w){
            layout->removeWidget(w);
            responseMap.remove(name);
            delete w;
        }
    }

}
