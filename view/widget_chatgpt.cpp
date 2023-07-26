#include "widget_chatgpt.h"


ParamHarmonicsWidget::ParamHarmonicsWidget(QWidget *parent) : QWidget(parent)
{
    freqMinSlider = new QSlider(Qt::Horizontal, this);
    freqMinSlider->setRange(20, 50000);
    freqMinSlider->setValue(20);

    freqMaxSlider = new QSlider(Qt::Horizontal, this);
    freqMaxSlider->setRange(20, 50000);
    freqMaxSlider->setValue(20000);

    durationSlider = new QSlider(Qt::Horizontal, this);
    durationSlider->setRange(1, 100);
    durationSlider->setValue(10);

    gainSlider = new QSlider(Qt::Horizontal, this);
    gainSlider->setRange(0, 100);
    gainSlider->setValue(50);

    durationSpinBox = new QDoubleSpinBox(this);
    durationSpinBox->setRange(0.1, 10.0);
    durationSpinBox->setSingleStep(0.1);
    durationSpinBox->setValue(1.0);

    gainSpinBox = new QDoubleSpinBox(this);
    gainSpinBox->setRange(0.0, 10.0);
    gainSpinBox->setSingleStep(0.1);
    gainSpinBox->setValue(1.0);

    applyButton = new QPushButton("Apply", this);

    connect(applyButton, &QPushButton::clicked, [this]() {
        ParamHarmonics params;
        params.freqMin = freqMinSlider->value();
        params.freqMax = freqMaxSlider->value();
        params.duration = durationSpinBox->value();
        params.gain = gainSpinBox->value();

        emit parameterSet(params);
    });

    updateFreqMaxSlider(freqMaxSlider->value());
}

void ParamHarmonicsWidget::updateFreqMaxSlider(int value)
{
    freqMaxSlider->setMinimum(freqMinSlider->value());
}

void ParamHarmonicsWidget::updateFreqMinSlider(int value)
{
    freqMinSlider->setMaximum(freqMaxSlider->value());
}

void ParamHarmonicsWidget::updateFreqMaxSpinBox(int value)
{
    freqMaxSlider->setMinimum(freqMinSlider->value());
}

void ParamHarmonicsWidget::updateFreqMinSpinBox(int value)
{
    freqMinSlider->setMaximum(freqMaxSlider->value());
}


ParamResponseWidget::ParamResponseWidget(QWidget *parent) : QWidget(parent)
{
    // Création des sliders pour modifier chaque paramètre
    freqMinSlider = new QSlider(Qt::Horizontal);
    freqMinSlider->setRange(20, 20000);
    freqMinSlider->setValue(20);

    freqMaxSlider = new QSlider(Qt::Horizontal);
    freqMaxSlider->setRange(20, 20000);
    freqMaxSlider->setValue(20000);

    durationSlider = new QSlider(Qt::Horizontal);
    durationSlider->setRange(1, 100);
    durationSlider->setValue(10);

    // Création du bouton "Valider"
    validateButton = new QPushButton("Valider");
    connect(validateButton, &QPushButton::clicked, this, &ParamResponseWidget::onValidateClicked);

    // Création du layout pour organiser les sliders et le bouton
    QFormLayout* layout = new QFormLayout;
    layout->addRow("Min Frequency:", freqMinSlider);
    layout->addRow("Max Frequency:", freqMaxSlider);
    layout->addRow("Duration:", durationSlider);
    layout->addWidget(validateButton);

    setLayout(layout);
}

ParamResponse ParamResponseWidget::getParamResponse() const
{
    // Récupération des valeurs des sliders
    ParamResponse paramResponse;
    paramResponse.freqMin = freqMinSlider->value();
    paramResponse.freqMax = freqMaxSlider->value();
    paramResponse.duration = static_cast<double>(durationSlider->value()) / 10.0;

    return paramResponse;
}

void ParamResponseWidget::onValidateClicked()
{
    // Émission du signal avec l'instance de ParamResponse modifiée
    emit paramResponseChanged(getParamResponse());
}













ParamSpectrogramWidget::ParamSpectrogramWidget(QWidget *parent) : QWidget(parent)
{
    freqMinSlider = new QSlider(Qt::Horizontal, this);
    freqMinSlider->setRange(20, 50000);
    freqMinSlider->setValue(20);

    freqMaxSlider = new QSlider(Qt::Horizontal, this);
    freqMaxSlider->setRange(20, 50000);
    freqMaxSlider->setValue(20000);

    durationSlider = new QSlider(Qt::Horizontal, this);
    durationSlider->setRange(10, 100); // Assuming your duration range is 0.1 to 10.0
    durationSlider->setValue(10);

    nbOctaveSpinBox = new QSpinBox(this);
    nbOctaveSpinBox->setRange(1, 10);
    nbOctaveSpinBox->setValue(3);

    resolutionSpinBox = new QSpinBox(this);
    resolutionSpinBox->setRange(1, 1000);
    resolutionSpinBox->setValue(512);

    applyButton = new QPushButton("Apply", this);

    connect(freqMinSlider, &QSlider::valueChanged, this, &ParamSpectrogramWidget::updateFreqMinSpinBox);
    connect(freqMaxSlider, &QSlider::valueChanged, this, &ParamSpectrogramWidget::updateFreqMaxSpinBox);
    connect(applyButton, &QPushButton::clicked, [this]() {
        ParamSpectrogram params;
        params.freqMin = freqMinSlider->value();
        params.freqMax = freqMaxSlider->value();
        params.duration = durationSlider->value() / 10.0; // Convert slider value back to double
        params.nb_octave = nbOctaveSpinBox->value();
        params.resolution = resolutionSpinBox->value();

        emit parameterSet(params);
    });

    updateFreqMaxSlider(freqMaxSlider->value());
}

void ParamSpectrogramWidget::updateFreqMaxSlider(int value)
{
    freqMaxSlider->setMinimum(freqMinSlider->value());
}

void ParamSpectrogramWidget::updateFreqMinSlider(int value)
{
    freqMinSlider->setMaximum(freqMaxSlider->value());
}

void ParamSpectrogramWidget::updateFreqMaxSpinBox(int value)
{
    freqMaxSlider->setMinimum(freqMinSlider->value());
}

void ParamSpectrogramWidget::updateFreqMinSpinBox(int value)
{
    freqMinSlider->setMaximum(freqMaxSlider->value());
}
