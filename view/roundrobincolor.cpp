#include "roundrobincolor.h"


RoundRobinColor::RoundRobinColor(std::initializer_list<QColor> &l):color(l),i(0){}
RoundRobinColor::RoundRobinColor():i(0){
    using namespace Qt;
    for(auto & i : {green,
                    red,
                    blue,
                    cyan,
                    magenta,
                    yellow,
                    black,
                    darkGray,
                    gray,
                    darkRed,
                    darkGreen,
                    darkBlue,
                    darkCyan,
                    darkMagenta,
                    darkYellow}){ //QColor::colorNames()){
        color.push_back(QColor(i));
    }
}
QColor RoundRobinColor::getNext(){
    i %= color.size();
    return color[i++];
}

