#include <qsoundfilewidget.h>



QSoundFileWidget::QSoundFileWidget(QWidget *parent) : QWidget(parent) {
    setupUI();

    connect(openFileButton, &QPushButton::clicked, this, &QSoundFileWidget::openFile);
    connect(setWindowButton, &QPushButton::clicked, this, &QSoundFileWidget::setWindow);
}

void QSoundFileWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    openFileButton = new QPushButton("Open File", this);
    infoLabel = new QLabel("No file loaded", this);
    sampleRateLabel = new QLabel("Sample Rate: ", this);
    durationLabel = new QLabel("Duration: ", this);

    startFrameSlider = new QSlider(Qt::Horizontal, this);
    numFramesSlider = new QSlider(Qt::Horizontal, this);
    startFrameValueLabel = new QLabel("0 (0.00 s)", this);
    numFramesValueLabel = new QLabel("1 (0.00 s)", this);
    setWindowButton = new QPushButton("Set Window", this);

    startFrameSlider->setRange(0, 0);
    numFramesSlider->setRange(1, 1);

    connect(startFrameSlider, &QSlider::valueChanged, this, [=](int value) {
        updateSliderLabel(startFrameSlider, startFrameValueLabel, currentSampleRate);
        ensureValidWindow();
    });
    connect(numFramesSlider, &QSlider::valueChanged, this, [=](int value) {
        updateSliderLabel(numFramesSlider, numFramesValueLabel, currentSampleRate);
        ensureValidWindow();
    });

    formLayout->addRow("Open File:", openFileButton);
    formLayout->addRow("Info:", infoLabel);
    formLayout->addRow("Sample Rate:", sampleRateLabel);
    formLayout->addRow("Duration:", durationLabel);
    formLayout->addRow("Start Frame:", startFrameSlider);
    formLayout->addRow("Start Frame Value:", startFrameValueLabel);
    formLayout->addRow("Number of Frames:", numFramesSlider);
    formLayout->addRow("Number of Frames Value:", numFramesValueLabel);
    formLayout->addRow("Set Window:", setWindowButton);
    channelLayout = new QVBoxLayout();
    formLayout->addRow("Input Channels",channelLayout);

    mainLayout->addLayout(formLayout);
}

void QSoundFileWidget::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Sound File", "", "Audio Files (*.wav *.flac *.mp3 *.aac *.ogg *.aiff *.wma *.m4a *.alac *.opus *.amr)");
    if (!filePath.isEmpty()) {
        emit openFileRequested(filePath);
    }
}

void QSoundFileWidget::handleNewFileOpen(int channels, int frames, int sampleRate) {
    infoLabel->setText("File Loaded Successfully");
    sampleRateLabel->setText(QString("%1").arg(sampleRate));
    durationLabel->setText(QString("%1 sec").arg(frames / float(sampleRate)));

    currentSampleRate = sampleRate;
    totalFrames = frames;
    updateChannelSelection(channels);
    startFrameSlider->setRange(0,totalFrames-1);
    numFramesSlider->setRange(1,totalFrames);
    updateSliderLabel(startFrameSlider, startFrameValueLabel, currentSampleRate);
    updateSliderLabel(numFramesSlider, numFramesValueLabel, currentSampleRate);
    ensureValidWindow();

}

void QSoundFileWidget::handleFileFailed(QString error) {
    infoLabel->setText("File Load Failed: " + error);
}

void QSoundFileWidget::setWindow() {
    emit setWindowRequested(startFrameSlider->value(), numFramesSlider->value());
}

void QSoundFileWidget::updateChannelSelection(int channels) {
    qDeleteAll(channelCheckBoxes);
    channelCheckBoxes.clear();
    channelLayout->update();

    for (int i = 0; i < channels; ++i) {
        QCheckBox *checkBox = new QCheckBox(QString("Channel %1").arg(i + 1), this);
        channelLayout->addWidget(checkBox);
        channelCheckBoxes.push_back(checkBox);

        connect(checkBox, &QCheckBox::toggled, this, [=](bool checked) {
            toggleChannel(i, checked);
        });
    }
}

void QSoundFileWidget::toggleChannel(int channel, bool checked){
    if(checked){
        emit setInputChannelRequested(channel);
    } else {
        emit unsetInputChannelRequested(channel);
    }
}

void QSoundFileWidget::updateSliderLabel(QSlider *slider, QLabel *label, int sampleRate) {
    int value = slider->value();
    double timeInSeconds = value / static_cast<double>(sampleRate);
    label->setText(QString("%1 (%2 s)").arg(value).arg(timeInSeconds, 0, 'f', 2));
}

void QSoundFileWidget::ensureValidWindow() {
    int start = startFrameSlider->value();
    int numFrames = numFramesSlider->value();

    if (start + numFrames > totalFrames) {
        numFrames = totalFrames - start;
        numFramesSlider->setValue(numFrames);
    }

    if (numFrames < 1) {
        numFrames = 1;
        numFramesSlider->setValue(numFrames);
    }

    updateSliderLabel(startFrameSlider, startFrameValueLabel, currentSampleRate);
    updateSliderLabel(numFramesSlider, numFramesValueLabel, currentSampleRate);
}

