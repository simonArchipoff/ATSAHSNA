#include "qsoundfilewidget.h"

QSoundFileWidget::QSoundFileWidget(QWidget *parent) : QWidget(parent) {
    setupUI();

    connect(openFileButton, &QPushButton::clicked, this, &QSoundFileWidget::openFile);
    connect(setWindowButton, &QPushButton::clicked, this, &QSoundFileWidget::setWindow);
}

void QSoundFileWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    openFileButton = new QPushButton("Open File", this);
    infoLabel = new QLabel("No file loaded", this);
    sampleRateLabel = new QLabel("Sample Rate: ", this);
    durationLabel = new QLabel("Duration: ", this);

    startFrameBox = new QSpinBox(this);
    numFramesBox = new QSpinBox(this);
    setWindowButton = new QPushButton("Set Window", this);

    startFrameBox->setRange(0, 1000000);
    numFramesBox->setRange(1, 1000000);

    channelsLayout = new QVBoxLayout();

    mainLayout->addWidget(openFileButton);
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(sampleRateLabel);
    mainLayout->addWidget(durationLabel);
    mainLayout->addLayout(channelsLayout);
    mainLayout->addWidget(startFrameBox);
    mainLayout->addWidget(numFramesBox);
    mainLayout->addWidget(setWindowButton);
}

void QSoundFileWidget::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Sound File", "", "Audio Files (*.wav *.flac *.mp3)");
    if (!filePath.isEmpty()) {
        emit openFileRequested(filePath);
    }
}

void QSoundFileWidget::handleNewFileOpen(int channels, int frames, int sampleRate) {
    infoLabel->setText("File Loaded Successfully");
    sampleRateLabel->setText(QString("Sample Rate: %1").arg(sampleRate));
    durationLabel->setText(QString("Duration: %1 sec").arg(frames / float(sampleRate)));
    updateChannelSelection(channels);
}

void QSoundFileWidget::handleFileFailed(QString error) {
    infoLabel->setText("File Load Failed: " + error);
}

void QSoundFileWidget::setWindow() {
    emit setWindowRequested(startFrameBox->value(), numFramesBox->value());
}

void QSoundFileWidget::updateChannelSelection(int channels) {
    QLayoutItem *child;
    while ((child = channelsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    channelCheckBoxes.clear();

    for (int i = 0; i < channels; ++i) {
        QCheckBox *checkBox = new QCheckBox(QString("Channel %1").arg(i + 1), this);
        channelCheckBoxes.push_back(checkBox);
        channelsLayout->addWidget(checkBox);
        connect(checkBox, &QCheckBox::toggled, this, [this, i](bool checked) { toggleChannel(i, checked); });
    }
}

void QSoundFileWidget::toggleChannel(int channel, bool checked) {
    if (checked) {
        emit setInputChannelRequested(channel);
    } else {
        emit unsetInputChannelRequested(channel);
    }
}

