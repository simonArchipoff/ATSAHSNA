#include "filedatasound.h"

FileDataSound::FileDataSound() {}

uint FileDataSound::numberInput() const{
    return 0;
}

uint FileDataSound::numberOutput() const{
    return 0;
}

uint FileDataSound::getSampleRate() const{
    return 0;
}

bool FileDataSound::isReady() const{
    return 0;
}

void FileDataSound::setFile(QString &filePath){
    decoder.setSource(QUrl(filePath));
    connect(&decoder, &QAudioDecoder::bufferReady, this, &FileDataSound::processBuffer);
    connect(&decoder, &QAudioDecoder::finished, this, &FileDataSound::onFinished);
    decoder.start();
}

void FileDataSound::processBuffer() {
    QAudioBuffer buffer = decoder.read();
    auto data = buffer.constData<double>();
    auto nb_channels = buffer.format().channelCount();
    QVector<double> s;
    s.resize(nb_channels, 0.0);

    for(int i = 0; i < buffer.frameCount();i++){
        for(int c = 0; c < nb_channels; c++)
            s[c] += data[i+c] * data[i+c];
    }
    for(auto & i : s){
        i /= buffer.frameCount();
        i = sqrt(i);
    }
    qDebug() << "rms " << s << "\n";
}

void FileDataSound::onFinished() {
    qDebug() << "Décodage terminé!";
}
