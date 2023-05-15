#include <QCoreApplication>
#include <QDebug>
#include "constants.h"
#include "commandlineparser.h"
#include <acquisition.h>






int main(int argc, char *argv[]){
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APPNAME);
    QCoreApplication::setApplicationVersion(APPVERS);
    CommandLineParser o(QCoreApplication::arguments());
    ParamFaust f;
    o.getParamBackend(&f);

    Backend * b;
    switch(o.backend){
    case FAUST:
        b = make_faust_backend(f);
        break;
    case JACK:
        break;
    case UNDEFBACKEND:
        break;
    }
    if(o.computation == RESPONSE) {
        ParamResponse p;
       /* auto r = compute<ImpulseResponse>(b, p);
        for(auto & o : r){
            FDFLOG foo(o.response);
            break;
        }*/
    } /*else if(o.computation == HARMONICS){
        ParamHarmonics p;
        auto r = compute<HarmonicResponse>(b,p);
        for(auto & o : r){
            FDFLOG foo(o.thdNoiseRate);;
            break;
        }
    }*/

    return 0;
}
