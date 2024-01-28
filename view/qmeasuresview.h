#pragma once
#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QLayout>
#include <Harmonics.h>
#include <Response.h>
#include <roundrobincolor.h>



class QResultView : public QWidget {
    Q_OBJECT
public:
    QResultView( QColor color, QString name, QWidget * parent=nullptr);

    bool getFrequencyDisplay() const{
        return frequency_display;
    }

    bool getTemporalDisplay() const{
        return temporal_display;
    }

    void setFrequencyDisplay(bool newFrequency_display);
    void setTemporalDisplay(bool newTemporal_display);


    QString getName() const;

signals:
    void copy(QString new_name);
    void remove();

    void temporalDisplay(const QString name, bool set);
    void frequencyDisplay(const QString name, bool set);

protected:
    bool frequency_display = true,
         temporal_display = false;
    QLayout * layout;
    QColor color;
    QString name;
    QFrame * colorFrame;
};

class QResultHarmonicsView : public QResultView {
    Q_OBJECT
public:
    QResultHarmonicsView(QColor color, const ResultHarmonics &r, QWidget * parent);
    void setSpectrogramDisplay(bool newSpectrogram_display);
signals:
    void spectrogramDisplay(const QString name, bool set);
protected:
    bool spectrogram_display = false;
    const ResultHarmonics result;
};

class QResultResponseView : public QResultView {
    Q_OBJECT
public:
    QResultResponseView(QColor color, const ResultResponse &r, QWidget * parent);
    void setTextDisplay(bool newText_display);
signals:
    void textDisplay(const QString name, bool set);
protected:
    bool text_display = true;
    const ResultResponse result;
};

inline QResultHarmonicsView * measureToQWidget(QColor color
                                              ,const ResultHarmonics &r
                                              ,QWidget * parent){
    return new QResultHarmonicsView(color, r, parent);
}
inline QResultResponseView * measureToQWidget(QColor color
                                             ,const ResultResponse &r
                                             ,QWidget * parent){
    return new QResultResponseView(color, r, parent);
}


class QMeasuresView : public QWidget{
    Q_OBJECT
public:
    QMeasuresView();
    template<typename T, typename R>
    R * addResult(const T &r){
        R * q = mesureToQWidget(rr_color.getNext(),r,this);
        layout->addWidget(q);
        return q;
    }

    void remove(QResultView *);

signals:
    void remove(QString);
protected:
    RoundRobinColor rr_color;
    QLayout * layout;
};
