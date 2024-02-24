#pragma once
#include <QObject>
#include <QList>

#include <Response.h>
#include <Harmonics.h>


#include <QWidget>

/*
template<typename R>
class QResultMeasureWidget : public R, public QWidget {
    Q_OBJECT
public:
    QResultMeasureWidget(const R & r):R(r){
    }

    void update(const R &r){
        *this = r;
    }
};



typedef QResultMeasureWidget<ResultResponse> QResultResponseWidget;
typedef QResultMeasureWidget<ResultHarmonics> QResultHarmonicsWidget;


class MeasureStore : QObject {
    Q_OBJECT
public:
    MeasureStore();

    void updateResponse(ResultResponse & r){
        if(responses.empty())
            responses.push_front(r);
        else
            responses[0] = r;
        emit currentResponseChanged(r);
    }
    void updateHarmonics(ResultHarmonics &r){
        if(harmonics.empty())
            harmonics.push_front(r);
        else
            harmonics[0] = r;
        emit currentHarmonicsChanged(r);
    }

    void saveCurrentResponse(){
        responses.push_back(responses[0]);
    }
    void saveCurrentHarmonics(){
        harmonics.push_back(harmonics[0]);
    }

    void removeResponse(int i){
        responses.removeAt(i);
    }
    void removeHarmonics(int i){
        harmonics.removeAt(i);
    }

signals:
    void currentResponseChanged(const ResultResponse & r);
    void currentHarmonicsChanged(const ResultHarmonics &r);

public:
    QList<ResultResponse> responses;
    QList<ResultHarmonics> harmonics;
};
*/




