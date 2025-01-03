#pragma once
#include <backend.h>
#include <QObject>

#include <QAudioDecoder>
#include <QByteArray>
#include <QUrl>

class FileDataSound:public QObject, public Backend {
    Q_OBJECT
public:
    FileDataSound();

    virtual uint numberInput() const override;
    virtual uint numberOutput() const override;
    virtual uint getSampleRate() const override;
    virtual bool isReady() const override;


    void setFile(QString &filePath);

private:

    void processBuffer();
    void onFinished();

    QAudioDecoder decoder;

};
