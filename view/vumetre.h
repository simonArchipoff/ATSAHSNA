#pragma once

#include <QWidget>

class VUMeter : public QWidget {
public:
    VUMeter(QWidget *parent = nullptr);
    void updateLevel(float l);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float level;
};
