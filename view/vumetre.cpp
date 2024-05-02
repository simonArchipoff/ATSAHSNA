#include "vumetre.h"

#include <QPainter>


VUMeter::VUMeter(QWidget *parent) : QWidget(parent), level(0) {
}

void VUMeter::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int width = this->width();
    int height = this->height();
    int barHeight = 20;
    int barWidth = width * level;
    qDebug() << level;

    painter.fillRect(0, 0, width, height, Qt::black);

    painter.fillRect(0, (height - barHeight) / 2, barWidth, barHeight, Qt::green);
}


void VUMeter::updateLevel(float l) {
    level = (l + 50) / (50 + 20);
    if(level < 0){
        level = 0;
    }
    if(level > 1){
        level= 1;
    }
    update();
}
