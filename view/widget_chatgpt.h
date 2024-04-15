#pragma once

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QFormLayout>
#include <QComboBox>
#include <Harmonics.h>
#include <Response.h>
#include <Spectrogram.h>
#include <qspinbox.h>

class SignalTypeSelector : public QWidget {
    Q_OBJECT

public:
    SignalTypeSelector(QWidget *parent = nullptr) : QWidget(parent) {
        // Créer un QComboBox
        comboBox = new QComboBox(this);

        // Ajouter les options à partir de l'énumération SIGNAL_TYPE
        comboBox->addItem("Chirp", QVariant(ParamResponse::CHIRP));
        comboBox->addItem("Dirac", QVariant(ParamResponse::DIRAC));
        comboBox->addItem("Undefined", QVariant(ParamResponse::UNDEFINED));

        // Connecter le signal currentIndexChanged à notre slot
        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SignalTypeSelector::handleSelectionChange);
    }

    ParamResponse::SIGNAL_TYPE getSelectedSignalType() const {
        // Récupérer la valeur sélectionnée dans le QComboBox
        return static_cast<ParamResponse::SIGNAL_TYPE>(comboBox->currentData().toInt());
    }

signals:
    void signalTypeChanged(ParamResponse::SIGNAL_TYPE type);

private slots:
    void handleSelectionChange(int index) {
        emit signalTypeChanged(static_cast<ParamResponse::SIGNAL_TYPE>(comboBox->itemData(index).toInt()));
    }

private:
    QComboBox *comboBox;
};

//this is chat gpt code
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
    QDoubleSpinBox* freqMinSlider;
    QDoubleSpinBox* freqMaxSlider;
    QDoubleSpinBox* durationSlider;
    SignalTypeSelector * signalSelector;
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
    QDoubleSpinBox *freqMinSlider;
    QDoubleSpinBox *freqMaxSlider;
    QDoubleSpinBox *durationSlider;
    QDoubleSpinBox *gainSlider;
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
