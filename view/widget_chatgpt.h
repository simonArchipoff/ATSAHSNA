#pragma once

#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>

#include <signalHarmonics.h>
#include <signalResponse.h>
#include <signalSpectrogram.h>


class ParamResponseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParamResponseWidget(QWidget *parent = nullptr);

    // Méthode pour récupérer l'instance de ParamResponse modifiée
    ParamResponse getParamResponse() const;

signals:
    // Signal émis lorsque l'utilisateur clique sur le bouton "Valider"
    void paramResponseChanged(const ParamResponse& paramResponse);

private slots:
    // Slot appelé lorsque l'utilisateur clique sur le bouton "Valider"
    void onValidateClicked();

private:
    QSlider* freqMinSlider;
    QSlider* freqMaxSlider;
    QSlider* durationSlider;
    QPushButton* validateButton;
};




class ParamHarmonicsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParamHarmonicsWidget(QWidget *parent = nullptr);

signals:
    void parameterSet(const ParamHarmonics &params);

private slots:
    void updateFreqMaxSlider(int value);
    void updateFreqMinSlider(int value);
    void updateFreqMaxSpinBox(int value);
    void updateFreqMinSpinBox(int value);

private:
    QSlider *freqMinSlider;
    QSlider *freqMaxSlider;
    QSlider *durationSlider;
    QSlider *gainSlider;
    QDoubleSpinBox *durationSpinBox;
    QDoubleSpinBox *gainSpinBox;
    QPushButton *applyButton;
};





class ParamSpectrogramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParamSpectrogramWidget(QWidget *parent = nullptr);

signals:
    void parameterSet(const ParamSpectrogram &params);

private slots:
    void updateFreqMaxSlider(int value);
    void updateFreqMinSlider(int value);
    void updateFreqMaxSpinBox(int value);
    void updateFreqMinSpinBox(int value);

private:
    QSlider *freqMinSlider;
    QSlider *freqMaxSlider;
    QSlider *durationSlider;
    QSpinBox *nbOctaveSpinBox;
    QSpinBox *resolutionSpinBox;
    QPushButton *applyButton;
};