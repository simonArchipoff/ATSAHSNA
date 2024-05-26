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
    int maxpos= width * max;
    painter.fillRect(0, 0, width, height, Qt::black);

    painter.fillRect(0, (height - barHeight) / 2, barWidth, barHeight, Qt::green);
    painter.fillRect(maxpos, (height - barHeight)/2, 3, barHeight,Qt::red);
}


void VUMeter::updateLevel(float l,float pmax) {
    const int mindb = 50, maxdb = 20;

    level = (l + mindb) / (mindb + maxdb);
    if(level < 0){
        level = 0;
    }
    if(level > 1){
        level= 1;
    }
    pmax = (pmax + mindb) / (mindb + maxdb);
    if(pmax>max){
        max=pmax;
    } else {
        max = 0.95 * max;
    }
    update();
}
