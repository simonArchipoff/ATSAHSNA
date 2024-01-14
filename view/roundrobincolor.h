#pragma once
#include <QColor>
#include <QVector>


class RoundRobinColor{
public:
    RoundRobinColor(std::initializer_list<QColor> &l);
    RoundRobinColor();
    QColor getNext();

protected:
    QVector<QColor> color;
    uint i;
};
