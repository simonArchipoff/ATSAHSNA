#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QCheckBox>
#include <QSlider>
#include <QFormLayout>
#include <QHBoxLayout>
#include <vector>

class QSoundFileWidget : public QWidget {
    Q_OBJECT

public:
    explicit QSoundFileWidget(QWidget *parent = nullptr);

signals:
    void openFileRequested(QString path);
    void setWindowRequested(int start, int nb);
    void setInputChannelRequested(int channel);
    void unsetInputChannelRequested(int channel);

public slots:
    void handleNewFileOpen(int channels, int frames, int sampleRate);
    void handleFileFailed(QString error);

private:
    void setupUI();
    void openFile();
    void setWindow();
    void updateChannelSelection(int channels);
    void toggleChannel(int channel, bool checked);
    void updateSliderLabel(QSlider *slider, QLabel *label, int sampleRate);
    void ensureValidWindow();

    QPushButton *openFileButton;
    QLabel *infoLabel;
    QLabel *sampleRateLabel;
    QLabel *durationLabel;
    QSlider *startFrameSlider;
    QSlider *numFramesSlider;
    QLabel *startFrameValueLabel;
    QLabel *numFramesValueLabel;
    QPushButton *setWindowButton;
    QVBoxLayout *channelLayout;
    std::vector<QCheckBox*> channelCheckBoxes;
    int currentSampleRate = 0;
    int totalFrames = 0;
};

