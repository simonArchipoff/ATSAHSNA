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
    QResultView(QColor color, QString name, QWidget * parent=nullptr);

    bool getFrequencyDisplay() const{
        return frequency_display;
    }

    bool getTemporalDisplay() const{
        return temporal_display;
    }

    void setFrequencyDisplay(bool newFrequency_display);
    void setTemporalDisplay(bool newTemporal_display);


    QString getName() const;
    QColor getColor() const{
        return color;
    }

signals:
    void copy(QString new_name);
    void remove(QString name);

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
    void updateResult(const ResultHarmonics &r);
    const ResultHarmonics & getResult() const {
        return result;
    }
signals:
    void spectrogramDisplay(const QString name, bool set);
    void updatedResult(const ResultHarmonics&r, QString name);
protected:
    bool spectrogram_display = false;
    ResultHarmonics result;
};

class QResultResponseView : public QResultView {
    Q_OBJECT
public:
    QResultResponseView(QColor color, const ResultResponse &r, QWidget * parent);
    void setTextDisplay(bool newText_display);
    void updateResult(const ResultResponse &r);
    const ResultResponse & getResult() const{
        return result;
    }
signals:
    void textDisplay(const QString name, bool set);
    void updatedResult( const ResultResponse&r, QString name);
protected:
    bool text_display = true;
    ResultResponse result;
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
    QMeasuresView(QWidget * parent);
    QResultHarmonicsView * addResult(const ResultHarmonics &r){
        QString n(r.name.c_str());
        if(harmonicsMap.contains(n)){
            harmonicsMap[n]->updateResult(r);
            return harmonicsMap[n];
        }else{
            auto q = measureToQWidget(rr_color.getNext(),r,this);
            layout->insertWidget(layout->count() -1, q);
            harmonicsMap.insert(q->getName(),q);
            return q;
        }
    }
    QResultResponseView * addResult(const ResultResponse &r){
        QString n(r.name.c_str());
        if(responseMap.contains(n)){
            responseMap[n]->updateResult(r);
            return responseMap[n];
        }else{
            auto q = measureToQWidget(rr_color.getNext(),r,this);
            layout->insertWidget(layout->count()-1,q);
            responseMap.insert(q->getName(),q);
            return q;
        }
    }

    void removeResult(QString name);

signals:
    void remove(QString);
protected:
    RoundRobinColor rr_color;
    QVBoxLayout * layout;
    QMap<QString,QResultHarmonicsView*> harmonicsMap;
    QMap<QString,QResultResponseView*> responseMap;
};
